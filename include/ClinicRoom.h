#ifndef CLINIC_ROOM_H
#define CLINIC_ROOM_H
#include "LinkedQueue.h"
#include "PriorityHeap.h"
namespace clinic {
class ClinicRoom {
public:
    ClinicRoom();
    ClinicRoom(const ClinicRoom&) = delete;
    ClinicRoom& operator=(const ClinicRoom&) = delete;
    void configure(int roomId, int specialtyId); // Chi khi phong rong.
    void enqueue(Patient* patient, bool usePriority);
    // Ham nay cap nhat status, assignedRoomId, startTime, finishTime.
    Patient* startNext(int minute, bool usePriority); // nullptr neu ban/rong.
    Patient* finishIfDue(int minute); // Doi status Completed, tra nguoi vua xong.
    void reset(); // Don hang, bo con tro dang kham; khong huy Patient.
    int id() const;
    int specialtyId() const;
    bool isBusy() const;
    int normalQueueSize() const;
    int priorityQueueSize() const;
    int waitingCount() const;
    const Patient* currentPatient() const;
private:
    int roomId_;
    int specialtyId_;
    LinkedQueue normal_;
    PriorityHeap priority_;
    Patient* current_;
};
} // namespace clinic
#endif
