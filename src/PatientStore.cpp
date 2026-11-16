#include "PatientStore.h"

#include <limits>
#include <new>
#include <stdexcept>

namespace clinic {

PatientStore::PatientStore() : head_(nullptr), size_(0) {}

PatientStore::~PatientStore() {
    clear();
}

bool PatientStore::add(const PatientInput& input, std::string& error) {
    error.clear();
    if (findById(input.id)) {
        error = "Ma benh nhan da ton tai: " + input.id;
        return false;
    }
    if (containsTicket(input.ticketNumber)) {
        error = "Ticket da ton tai: " + std::to_string(input.ticketNumber);
        return false;
    }
    if (size_ == std::numeric_limits<int>::max()) {
        error = "Kho da dat gioi han so benh nhan.";
        return false;
    }

    Node* previous = nullptr;
    Node* current = head_;
    while (current &&
           (current->patient.input.arrivalTime < input.arrivalTime ||
            (current->patient.input.arrivalTime == input.arrivalTime &&
             current->patient.input.ticketNumber < input.ticketNumber))) {
        previous = current;
        current = current->next;
    }

    Node* node = nullptr;
    try {
        node = new Node{Patient{input}, current};
    } catch (const std::bad_alloc&) {
        error = "Khong du bo nho de them benh nhan.";
        return false;
    }

    if (previous)
        previous->next = node;
    else
        head_ = node;
    ++size_;
    return true;
}

const Patient* PatientStore::findById(const std::string& id) const {
    for (const Node* node = head_; node; node = node->next)
        if (node->patient.input.id == id) return &node->patient;
    return nullptr;
}

bool PatientStore::containsTicket(int ticketNumber) const {
    for (const Node* node = head_; node; node = node->next)
        if (node->patient.input.ticketNumber == ticketNumber) return true;
    return false;
}

int PatientStore::nextTicketNumber() const {
    int maximum = 0;
    for (const Node* node = head_; node; node = node->next)
        if (node->patient.input.ticketNumber > maximum)
            maximum = node->patient.input.ticketNumber;
    if (maximum == std::numeric_limits<int>::max())
        throw std::overflow_error("Da het mien ticket int.");
    return maximum + 1;
}

int PatientStore::size() const { return size_; }

bool PatientStore::isEmpty() const { return head_ == nullptr; }

const PatientStore::Node* PatientStore::head() const { return head_; }

void PatientStore::resetStates() {
    for (Node* node = head_; node; node = node->next)
        resetPatientState(node->patient);
}

void PatientStore::clear() {
    while (head_) {
        Node* node = head_;
        head_ = head_->next;
        delete node;
    }
    size_ = 0;
}

} // namespace clinic
