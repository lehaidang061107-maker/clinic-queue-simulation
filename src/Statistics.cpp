#include "Statistics.h"
#include <memory>
#include <sstream>
#include <stdexcept>

namespace clinic {

namespace {

bool comesBeforeInReport(const Patient* a, const Patient* b) {
    int waitA = waitingTime(*a);
    int waitB = waitingTime(*b);
    if (waitA != waitB) {
        return waitA > waitB; // Thoi gian cho giam dan
    }
    return a->input.ticketNumber < b->input.ticketNumber; // Hoa thi ticket tang dan
}

void mergePatients(const Patient** arr, const Patient** temp, int left, int mid, int right) {
    int i = left;
    int j = mid + 1;
    int k = left;

    while (i <= mid && j <= right) {
        if (comesBeforeInReport(arr[i], arr[j])) {
            temp[k++] = arr[i++];
        } else {
            temp[k++] = arr[j++];
        }
    }
    while (i <= mid) {
        temp[k++] = arr[i++];
    }
    while (j <= right) {
        temp[k++] = arr[j++];
    }
    for (int idx = left; idx <= right; ++idx) {
        arr[idx] = temp[idx];
    }
}

void mergeSortPatients(const Patient** arr, const Patient** temp, int left, int right) {
    if (left >= right) {
        return;
    }
    int mid = left + (right - left) / 2;
    mergeSortPatients(arr, temp, left, mid);
    mergeSortPatients(arr, temp, mid + 1, right);
    mergePatients(arr, temp, left, mid, right);
}

// PatientStore::Node chua truc tiep Patient (khong phai con tro).
const Patient* extractPatient(const PatientStore::Node* node) {
    return &node->patient;
}

} // namespace

Statistics::Statistics()
    : rooms_(nullptr),
      roomCount_(0),
      maxSystemWaiting_(0),
      completedCount_(0) {}

Statistics::~Statistics() {
    delete[] rooms_;
    rooms_ = nullptr;
    roomCount_ = 0;
}

void Statistics::reset(int roomCount) {
    if (roomCount < 0) {
        throw std::invalid_argument("Statistics::reset: roomCount must be >= 0");
    }

    // Cap phat mang moi truoc khi thay doi trang thai cu (dam bao strong exception safety).
    RoomStats* nextRooms = nullptr;
    if (roomCount > 0) {
        nextRooms = new RoomStats[roomCount];
        for (int i = 0; i < roomCount; ++i) {
            nextRooms[i].roomId = i + 1;
            nextRooms[i].maxWaiting = 0;
        }
    }

    delete[] rooms_;
    rooms_ = nextRooms;
    roomCount_ = roomCount;

    for (int i = 0; i < kPriorityCount; ++i) {
        priority_[i] = PriorityStats{};
    }

    maxSystemWaiting_ = 0;
    completedCount_ = 0;
}

void Statistics::recordStart(const Patient& patient) {
    if (patient.startTime < 0) {
        return;
    }
    int idx = static_cast<int>(patient.input.priority);
    if (idx < 0 || idx >= kPriorityCount) {
        return;
    }

    int wait = waitingTime(patient);
    if (priority_[idx].startedCount == 0 || wait > priority_[idx].maxWait) {
        priority_[idx].maxWait = wait;
    }
    priority_[idx].startedCount += 1;
    priority_[idx].totalWait += wait;
}

void Statistics::recordCompletion() {
    completedCount_ += 1;
}

void Statistics::observeQueues(int roomIndex, int roomWaiting, int systemWaiting) {
    if (rooms_ != nullptr && roomIndex >= 0 && roomIndex < roomCount_) {
        if (roomWaiting > rooms_[roomIndex].maxWaiting) {
            rooms_[roomIndex].maxWaiting = roomWaiting;
        }
    }
    if (systemWaiting > maxSystemWaiting_) {
        maxSystemWaiting_ = systemWaiting;
    }
}

const PriorityStats& Statistics::byPriority(Priority priority) const {
    int idx = static_cast<int>(priority);
    if (idx < 0 || idx >= kPriorityCount) {
        throw std::out_of_range("Statistics::byPriority: invalid priority");
    }
    return priority_[idx];
}

const RoomStats& Statistics::byRoom(int roomIndex) const {
    if (rooms_ == nullptr || roomIndex < 0 || roomIndex >= roomCount_) {
        throw std::out_of_range("Statistics::byRoom: roomIndex out of range");
    }
    return rooms_[roomIndex];
}

int Statistics::maxSystemWaiting() const {
    return maxSystemWaiting_;
}

int Statistics::completedCount() const {
    return completedCount_;
}

int Statistics::totalStartedCount() const {
    int total = 0;
    for (int i = 0; i < kPriorityCount; ++i) {
        total += priority_[i].startedCount;
    }
    return total;
}

long long Statistics::totalWaitingTime() const {
    long long total = 0;
    for (int i = 0; i < kPriorityCount; ++i) {
        total += priority_[i].totalWait;
    }
    return total;
}

double Statistics::averageWait() const {
    const int started = totalStartedCount();
    if (started == 0) {
        return 0.0;
    }
    return static_cast<double>(totalWaitingTime()) / started;
}

std::string Statistics::formatAverageWait() const {
    if (totalStartedCount() == 0) {
        return "Chua co du lieu";
    }
    std::ostringstream out;
    out << averageWait();
    return out.str();
}

const Patient** makeSortedWaitReport(const PatientStore& store, int& count) {
    count = 0;
    for (const auto* node = store.head(); node != nullptr; node = node->next) {
        const Patient* p = extractPatient(node);
        if (p != nullptr && p->startTime >= 0) {
            ++count;
        }
    }

    if (count == 0) {
        return nullptr;
    }

    std::unique_ptr<const Patient*[]> report(new const Patient*[count]);
    int pos = 0;
    for (const auto* node = store.head(); node != nullptr; node = node->next) {
        const Patient* p = extractPatient(node);
        if (p != nullptr && p->startTime >= 0) {
            report[pos++] = p;
        }
    }

    std::unique_ptr<const Patient*[]> temp(new const Patient*[count]);
    mergeSortPatients(report.get(), temp.get(), 0, count - 1);

    return report.release();
}

} // namespace clinic