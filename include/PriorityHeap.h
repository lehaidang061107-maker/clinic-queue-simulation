#ifndef CLINIC_PRIORITY_HEAP_H
#define CLINIC_PRIORITY_HEAP_H
#include "Patient.h"
namespace clinic {
class PriorityHeap {
public:
    PriorityHeap();
    ~PriorityHeap();
    PriorityHeap(const PriorityHeap&) = delete;
    PriorityHeap& operator=(const PriorityHeap&) = delete;
    void push(Patient* patient); // Khong nullptr; tu tang dung luong khi can.
    Patient* pop(); // nullptr neu rong.
    const Patient* peek() const;
    bool isEmpty() const;
    int size() const;
    void clear(); // Khong huy Patient; size=0, co the giu dung luong.
private:
    Patient** items_;
    int size_;
    int capacity_;
    // So sanh (priority, arrivalTime, ticketNumber) tang dan.
    static bool comesBefore(const Patient* lhs, const Patient* rhs);
    void grow();
    void siftUp(int index);
    void siftDown(int index);
};
} // namespace clinic
#endif
