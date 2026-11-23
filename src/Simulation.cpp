#include "Simulation.h"
#include <limits>
#include <memory>
#include <new>
#include <stdexcept>

namespace clinic {
Simulation::Simulation(PatientStore& store)
    : store_(store), rooms_(nullptr), roomCount_(0), nextMinute_(0), initialized_(false) {}

Simulation::~Simulation() { delete[] rooms_; }

bool Simulation::initialize(const SimulationConfig& config, std::string& error) {
    if (!validateSimulationConfig(config, error)) return false;
    // Conservative bound: all service could take place sequentially after closing-1.
    int remaining = std::numeric_limits<int>::max() - config.closingTime;
    for (const PatientStore::Node* node = store_.head_; node; node = node->next) {
        if (!validatePatientInput(node->patient.input, config.closingTime, error)) return false;
        if (node->patient.input.serviceDuration > remaining) {
            error = "Tong thoi luong co the lam tran dong ho mo phong.";
            return false;
        }
        remaining -= node->patient.input.serviceDuration;
    }
    int count = 0;
    for (int n : config.roomsPerSpecialty) count += n;
    try {
        std::unique_ptr<ClinicRoom[]> replacement(new ClinicRoom[count]);
        int index = 0;
        for (int specialty = 1; specialty <= kSpecialtyCount; ++specialty) {
            for (int n = 0; n < config.roomsPerSpecialty[specialty - 1]; ++n) {
                replacement[index].configure(index + 1, specialty);
                ++index;
            }
        }
        // Statistics::reset must allocate before changing its existing state (B contract).
        statistics_.reset(count);
        delete[] rooms_; // Release borrowed references before resetting patients.
        rooms_ = replacement.release();
        roomCount_ = count;
        store_.resetStates();
        config_ = config;
        nextMinute_ = 0;
        initialized_ = true;
        error.clear();
        return true;
    } catch (const std::bad_alloc&) {
        error = "Khong du bo nho de khoi tao mo phong.";
        return false;
    }
}

bool Simulation::admit(const std::string& id, const std::string& name, int age,
                       bool emergency, int specialtyId, int duration, std::string& error) {
    error.clear();
    if (!initialized_ || nextMinute_ >= config_.closingTime) {
        error = "Chua khoi tao hoac da dong tiep nhan.";
        return false;
    }
    try {
        // Check before calling C's max+1 helper, including non-contiguous tickets.
        int maxTicket = 0;
        int remaining = std::numeric_limits<int>::max() - config_.closingTime;
        for (const PatientStore::Node* node = store_.head_; node; node = node->next) {
            if (node->patient.input.ticketNumber > maxTicket)
                maxTicket = node->patient.input.ticketNumber;
            remaining -= node->patient.input.serviceDuration;
        }
        if (maxTicket == std::numeric_limits<int>::max()) {
            error = "Da het mien ticket int.";
            return false;
        }
        PatientInput input{id, name, age, classifyPriority(age, emergency), specialtyId,
                           store_.nextTicketNumber(), nextMinute_, duration};
        if (!validatePatientInput(input, config_.closingTime, error)) return false;
        if (duration > remaining) {
            error = "Tong thoi luong co the lam tran dong ho mo phong.";
            return false;
        }
        if (!store_.add(input, error)) return false;
        // Xep phong ngay: phan tat ca nguoi den phut nay theo thu tu (arrival, ticket),
        // giong het buoc "tiep nhan nguoi den" cua step(). Hoan tat ca dang kham khong
        // doi so cho, nen chon phong truoc buoc hoan tat cho cung ket qua.
        try {
            assignArrivals(nextMinute_);
        } catch (...) {
            // Nguoi moi da vao kho nhung phong co the da xep do dang: buoc khoi tao lai.
            initialized_ = false;
            error = "Khong xep duoc phong; hay khoi tao lai mo phong (menu 3).";
            return false;
        }
        return true;
    } catch (const std::bad_alloc&) {
        error = "Khong du bo nho de tiep nhan.";
    } catch (const std::overflow_error& failure) {
        error = failure.what();
    }
    return false;
}

int Simulation::selectRoom(int specialtyId) const {
    int best = -1;
    for (int i = 0; i < roomCount_; ++i) {
        if (rooms_[i].specialtyId() != specialtyId) continue;
        if (best < 0 || rooms_[i].waitingCount() < rooms_[best].waitingCount() ||
            (rooms_[i].waitingCount() == rooms_[best].waitingCount() && rooms_[i].id() < rooms_[best].id()))
            best = i;
    }
    if (best < 0) throw std::logic_error("Khong co phong dung khoa.");
    return best;
}

void Simulation::assignArrivals(int minute) {
    // C maintains (arrival, ticket) order, also when admitting between steps.
    for (PatientStore::Node* node = store_.head_; node; node = node->next) {
        Patient& patient = node->patient;
        if (patient.status != PatientStatus::NotArrived || patient.input.arrivalTime != minute)
            continue;
        const int index = selectRoom(patient.input.specialtyId);
        rooms_[index].enqueue(&patient, config_.usePriority);
        patient.assignedRoomId = rooms_[index].id();
        patient.status = PatientStatus::Waiting;
    }
}

void Simulation::observeAllQueues() {
    int total = 0;
    for (int i = 0; i < roomCount_; ++i) total += rooms_[i].waitingCount();
    for (int i = 0; i < roomCount_; ++i)
        statistics_.observeQueues(i, rooms_[i].waitingCount(), total);
}

bool Simulation::step() {
    if (!initialized_ || isFinished()) return false;
    try {
        for (int i = 0; i < roomCount_; ++i)
            if (rooms_[i].finishIfDue(nextMinute_)) statistics_.recordCompletion();
        assignArrivals(nextMinute_);
        observeAllQueues();
        for (int i = 0; i < roomCount_; ++i) {
            Patient* patient = rooms_[i].startNext(nextMinute_, config_.usePriority);
            if (patient) statistics_.recordStart(*patient);
        }
        observeAllQueues();
        ++nextMinute_;
        return true;
    } catch (...) {
        // The minute may be partially processed. Retrying could double-count events.
        initialized_ = false;
        throw;
    }
}

bool Simulation::isFinished() const {
    if (!initialized_ || nextMinute_ < config_.closingTime) return false;
    for (const PatientStore::Node* node = store_.head_; node; node = node->next)
        if (node->patient.status != PatientStatus::Completed) return false;
    for (int i = 0; i < roomCount_; ++i)
        if (rooms_[i].isBusy() || rooms_[i].waitingCount() != 0) return false;
    return true;
}
int Simulation::nextMinute() const { return nextMinute_; }
int Simulation::lastProcessedMinute() const { return nextMinute_ - 1; }
int Simulation::roomCount() const { return roomCount_; }
const ClinicRoom& Simulation::roomAt(int index) const {
    if (index < 0 || index >= roomCount_) throw std::out_of_range("Chi so phong khong hop le.");
    return rooms_[index];
}
const Statistics& Simulation::statistics() const { return statistics_; }
const SimulationConfig& Simulation::config() const { return config_; }
} // namespace clinic
