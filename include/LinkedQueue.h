#ifndef CLINIC_LINKED_QUEUE_H
#define CLINIC_LINKED_QUEUE_H
#include "Patient.h"
namespace clinic {
class LinkedQueue {
public:
    LinkedQueue();
    ~LinkedQueue();
    LinkedQueue(const LinkedQueue&) = delete;
    LinkedQueue& operator=(const LinkedQueue&) = delete;
    void enqueue(Patient* patient); // Khong nullptr, khong so huu Patient.
    Patient* dequeue(); // nullptr neu rong.
    const Patient* peek() const;
    bool isEmpty() const;
    int size() const;
    void clear(); // Chi huy node.
private:
    struct Node { Patient* patient; Node* next; };
    Node* front_;
    Node* rear_;
    int size_;
};
} // namespace clinic
#endif
