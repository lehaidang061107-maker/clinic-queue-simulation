
#include "Statistics.h"

namespace clinic {

namespace {

bool comesBeforeInReport(const Patient* a, const Patient* b) {
    int waitA = waitingTime(*a);
    int waitB = waitingTime(*b);
    if (waitA != waitB) {
        return waitA > waitB; 
    }
    return a->input.ticketNumber < b->input.ticketNumber; 
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

} 



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
    for (int i = 0; i < kPriorityCount; ++i) {
        priority_[i] = PriorityStats{};
    }

    delete[] rooms_;
    rooms_ = nullptr;
    roomCount_ = 0;

    if (roomCount > 0) {
        roomCount_ = roomCount;
        rooms_ = new RoomStats[roomCount_];
        for (int i = 0; i < roomCount_; ++i) {
            rooms_[i].roomId = i + 1;
            rooms_[i].maxWaiting = 0;
        }
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
        return priority_[0];
    }
    return priority_[idx];
}

const RoomStats& Statistics::byRoom(int roomIndex) const {
    return rooms_[roomIndex];
}

int Statistics::maxSystemWaiting() const {
    return maxSystemWaiting_;
}

int Statistics::completedCount() const {
    return completedCount_;
}





const Patient** makeSortedWaitReport(const PatientStore& store, int& count) {
    count = 0;
    int totalPatients = store.size(); /

    for (int i = 0; i < totalPatients; ++i) {
        const Patient* p = store.at(i); 
        if (p != nullptr && p->startTime >= 0) {
            ++count;
        }
    }

    if (count == 0) {
        return nullptr;
    }

    
    
    const Patient** report = new const Patient*[count];
    int pos = 0;
    for (int i = 0; i < totalPatients; ++i) {
        const Patient* p = store.at(i); 
        if (p != nullptr && p->startTime >= 0) {
            report[pos++] = p;
        }
    }

    const Patient** temp = new const Patient*[count];
    mergeSortPatients(report, temp, 0, count - 1);
    delete[] temp;

    return report;
}

} 
