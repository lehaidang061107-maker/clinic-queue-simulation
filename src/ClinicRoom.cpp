#include "ClinicRoom.h"

namespace clinic {

ClinicRoom::ClinicRoom()
    : roomId_(-1), specialtyId_(-1), normal_(), priority_(), current_(nullptr) {}

void ClinicRoom::configure(int roomId, int specialtyId) {
    if (!isBusy() && waitingCount() == 0) {
        roomId_ = roomId;
        specialtyId_ = specialtyId;
    }
}

void ClinicRoom::enqueue(Patient* patient, bool usePriority) {
    if (patient == nullptr) {
        return;
    }
    if (usePriority && (patient->input.priority == Priority::Emergency ||
                        patient->input.priority == Priority::Preferential)) {
        priority_.push(patient);
    } else {
        normal_.enqueue(patient);
    }
}

Patient* ClinicRoom::startNext(int minute, bool usePriority) {
    if (isBusy() || waitingCount() == 0) {
        return nullptr;
    }

    Patient* nextPatient = nullptr;
    if (usePriority) {
        if (!priority_.isEmpty()) {
            nextPatient = priority_.pop();
        } else {
            nextPatient = normal_.dequeue();
        }
    } else {
        if (!normal_.isEmpty()) {
            nextPatient = normal_.dequeue();
        } else {
            nextPatient = priority_.pop();
        }
    }

    if (nextPatient == nullptr) {
        return nullptr;
    }

    current_ = nextPatient;
    current_->status = PatientStatus::InService;
    current_->assignedRoomId = roomId_;
    current_->startTime = minute;
    current_->finishTime = minute + current_->input.serviceDuration;
    return current_;
}

Patient* ClinicRoom::finishIfDue(int minute) {
    if (current_ != nullptr && minute >= current_->finishTime) {
        Patient* finished = current_;
        finished->status = PatientStatus::Completed;
        current_ = nullptr;
        return finished;
    }
    return nullptr;
}

void ClinicRoom::reset() {
    normal_.clear();
    priority_.clear();
    current_ = nullptr;
}

int ClinicRoom::id() const {
    return roomId_;
}

int ClinicRoom::specialtyId() const {
    return specialtyId_;
}

bool ClinicRoom::isBusy() const {
    return current_ != nullptr;
}

int ClinicRoom::normalQueueSize() const {
    return normal_.size();
}

int ClinicRoom::priorityQueueSize() const {
    return priority_.size();
}

int ClinicRoom::waitingCount() const {
    return normal_.size() + priority_.size();
}

const Patient* ClinicRoom::currentPatient() const {
    return current_;
}

} // namespace clinic
