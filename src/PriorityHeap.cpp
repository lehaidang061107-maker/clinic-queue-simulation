#include "PriorityHeap.h"
#include <climits>
#include <stdexcept>

namespace clinic {

PriorityHeap::PriorityHeap() : items_(nullptr), size_(0), capacity_(0) {
    capacity_ = 16;
    items_ = new Patient*[capacity_];
}

PriorityHeap::~PriorityHeap() {
    delete[] items_;
    items_ = nullptr;
    size_ = 0;
    capacity_ = 0;
}

bool PriorityHeap::comesBefore(const Patient* lhs, const Patient* rhs) {
    int lhsPrio = static_cast<int>(lhs->input.priority);
    int rhsPrio = static_cast<int>(rhs->input.priority);
    if (lhsPrio != rhsPrio) {
        return lhsPrio < rhsPrio;
    }
    if (lhs->input.arrivalTime != rhs->input.arrivalTime) {
        return lhs->input.arrivalTime < rhs->input.arrivalTime;
    }
    return lhs->input.ticketNumber < rhs->input.ticketNumber;
}

void PriorityHeap::grow() {
    if (capacity_ > INT_MAX / 2) {
        throw std::overflow_error("PriorityHeap::grow: capacity overflow");
    }
    int newCapacity = (capacity_ == 0) ? 16 : capacity_ * 2;
    Patient** newItems = new Patient*[newCapacity];
    for (int i = 0; i < size_; ++i) {
        newItems[i] = items_[i];
    }
    delete[] items_;
    items_ = newItems;
    capacity_ = newCapacity;
}

void PriorityHeap::siftUp(int index) {
    while (index > 0) {
        int parent = (index - 1) / 2;
        if (comesBefore(items_[index], items_[parent])) {
            Patient* temp = items_[index];
            items_[index] = items_[parent];
            items_[parent] = temp;
            index = parent;
        } else {
            break;
        }
    }
}

void PriorityHeap::siftDown(int index) {
    while (true) {
        int smallest = index;
        int left = 2 * index + 1;
        int right = 2 * index + 2;

        if (left < size_ && comesBefore(items_[left], items_[smallest])) {
            smallest = left;
        }
        if (right < size_ && comesBefore(items_[right], items_[smallest])) {
            smallest = right;
        }

        if (smallest != index) {
            Patient* temp = items_[index];
            items_[index] = items_[smallest];
            items_[smallest] = temp;
            index = smallest;
        } else {
            break;
        }
    }
}

void PriorityHeap::push(Patient* patient) {
    if (patient == nullptr) {
        throw std::invalid_argument("PriorityHeap::push: patient must not be nullptr");
    }
    if (size_ == capacity_) {
        grow();
    }
    items_[size_] = patient;
    siftUp(size_);
    ++size_;
}

Patient* PriorityHeap::pop() {
    if (size_ == 0) {
        return nullptr;
    }
    Patient* top = items_[0];
    --size_;
    if (size_ > 0) {
        items_[0] = items_[size_];
        siftDown(0);
    }
    return top;
}

const Patient* PriorityHeap::peek() const {
    if (size_ == 0) {
        return nullptr;
    }
    return items_[0];
}

bool PriorityHeap::isEmpty() const {
    return size_ == 0;
}

int PriorityHeap::size() const {
    return size_;
}

void PriorityHeap::clear() {
    size_ = 0;
}

} // namespace clinic
