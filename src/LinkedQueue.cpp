#include "LinkedQueue.h"

namespace clinic {

LinkedQueue::LinkedQueue() : front_(nullptr), rear_(nullptr), size_(0) {}

LinkedQueue::~LinkedQueue() {
    clear();
}

void LinkedQueue::enqueue(Patient* patient) {
    if (patient == nullptr) {
        return;
    }
    Node* newNode = new Node{patient, nullptr};
    if (rear_ == nullptr) {
        front_ = newNode;
        rear_ = newNode;
    } else {
        rear_->next = newNode;
        rear_ = newNode;
    }
    ++size_;
}

Patient* LinkedQueue::dequeue() {
    if (isEmpty()) {
        return nullptr;
    }
    Node* temp = front_;
    Patient* patient = temp->patient;
    front_ = front_->next;
    if (front_ == nullptr) {
        rear_ = nullptr;
    }
    delete temp;
    --size_;
    return patient;
}

const Patient* LinkedQueue::peek() const {
    if (isEmpty()) {
        return nullptr;
    }
    return front_->patient;
}

bool LinkedQueue::isEmpty() const {
    return size_ == 0;
}

int LinkedQueue::size() const {
    return size_;
}

void LinkedQueue::clear() {
    while (front_ != nullptr) {
        Node* temp = front_;
        front_ = front_->next;
        delete temp; // Chi giai phong Node, khong delete Patient*
    }
    rear_ = nullptr;
    size_ = 0;
}

} // namespace clinic
