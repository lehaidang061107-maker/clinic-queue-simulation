#include "Statistics.h"
#include <stdexcept>
#include <type_traits>

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

template <typename NodePtr>
const Patient* extractPatient(NodePtr node) {
    if constexpr (std::is_pointer_v<decltype(node->patient)>) {
        return node->patient;
    } else {
        return &node->patient;
    }
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

    const Patient** report = new const Patient*[count];
    int pos = 0;
    for (const auto* node = store.head(); node != nullptr; node = node->next) {
        const Patient* p = extractPatient(node);
        if (p != nullptr && p->startTime >= 0) {
            report[pos++] = p;
        }
    }

    const Patient** temp = new const Patient*[count];
    mergeSortPatients(report, temp, 0, count - 1);
    delete[] temp;

    return report;
}

} // namespace clinic
