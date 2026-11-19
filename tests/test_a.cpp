// Kiem thu thanh vien A: LinkedQueue va ClinicRoom.
// Mac dinh can PriorityHeap that (ClinicRoom dung heap). Khi heap chua co,
// bien dich voi -DTEST_A_QUEUE_ONLY de chi chay phan FIFO.
#include "LinkedQueue.h"
#ifndef TEST_A_QUEUE_ONLY
#include "ClinicRoom.h"
#endif
#include "Patient.h"

#include <cassert>
#include <iostream>
#include <string>

using namespace clinic;

namespace {

Patient makePatient(const std::string& id, int ticket, int arrival,
                    Priority priority = Priority::Normal, int duration = 5) {
    Patient p;
    p.input.id = id;
    p.input.fullName = "Patient " + id;
    p.input.age = 30;
    p.input.priority = priority;
    p.input.specialtyId = 1;
    p.input.ticketNumber = ticket;
    p.input.arrivalTime = arrival;
    p.input.serviceDuration = duration;
    p.status = PatientStatus::Waiting;
    return p;
}

void checkQueueEmpty() {
    LinkedQueue q;
    assert(q.isEmpty() && q.size() == 0);
    assert(q.peek() == nullptr);
    assert(q.dequeue() == nullptr);
    q.enqueue(nullptr); // Bo qua, khong tao node.
    assert(q.isEmpty() && q.size() == 0);
    q.clear(); // Clear hang rong khong loi.
    assert(q.isEmpty());
    std::cout << "PASS LinkedQueue empty queue, nullptr ignored, clear on empty\n";
}

void checkQueueSingleAndRefill() {
    Patient a = makePatient("BN01", 1, 0);
    Patient b = makePatient("BN02", 2, 1);
    LinkedQueue q;
    q.enqueue(&a);
    assert(q.size() == 1 && q.peek() == &a);
    assert(q.dequeue() == &a);
    assert(q.isEmpty() && q.peek() == nullptr && q.dequeue() == nullptr);
    // Lay nguoi cuoi phai dua rear ve nullptr: them lai phai hoat dong dung.
    q.enqueue(&b);
    assert(q.size() == 1 && q.peek() == &b);
    q.enqueue(&a);
    assert(q.dequeue() == &b && q.dequeue() == &a && q.isEmpty());
    std::cout << "PASS LinkedQueue single element, drain, refill after empty\n";
}

void checkQueueFifoOrder() {
    Patient p1 = makePatient("BN01", 1, 0);
    Patient p2 = makePatient("BN02", 2, 1);
    Patient p3 = makePatient("BN03", 3, 2);
    Patient p4 = makePatient("BN04", 4, 3);
    LinkedQueue q;
    q.enqueue(&p1);
    q.enqueue(&p2);
    q.enqueue(&p3);
    assert(q.size() == 3 && q.peek() == &p1);
    assert(q.dequeue() == &p1);
    q.enqueue(&p4); // Them giua chung van xep cuoi.
    assert(q.size() == 3);
    assert(q.dequeue() == &p2);
    assert(q.dequeue() == &p3);
    assert(q.dequeue() == &p4);
    assert(q.isEmpty() && q.dequeue() == nullptr);
    std::cout << "PASS LinkedQueue FIFO BN01 -> BN02 -> BN03, BN04 appended later\n";
}

void checkQueueClearKeepsPatients() {
    Patient p1 = makePatient("BN01", 1, 0);
    Patient p2 = makePatient("BN02", 2, 1);
    LinkedQueue q;
    q.enqueue(&p1);
    q.enqueue(&p2);
    q.clear();
    assert(q.isEmpty() && q.size() == 0 && q.peek() == nullptr);
    // Patient van con nguyen (clear chi huy node).
    assert(p1.input.id == "BN01" && p2.input.id == "BN02");
    q.enqueue(&p2);
    assert(q.dequeue() == &p2 && q.isEmpty());
    {
        LinkedQueue scoped;
        scoped.enqueue(&p1);
        scoped.enqueue(&p2);
    } // Destructor huy node, khong huy Patient.
    assert(p1.input.ticketNumber == 1 && p2.input.ticketNumber == 2);
    std::cout << "PASS LinkedQueue clear/destructor free nodes only, reuse after clear\n";
}

#ifndef TEST_A_QUEUE_ONLY

void checkRoomEmptyAndConfigure() {
    ClinicRoom room;
    room.configure(101, 1);
    assert(room.id() == 101 && room.specialtyId() == 1);
    assert(!room.isBusy() && room.waitingCount() == 0);
    assert(room.currentPatient() == nullptr);
    assert(room.startNext(0, true) == nullptr);
    assert(room.startNext(0, false) == nullptr);
    assert(room.finishIfDue(100) == nullptr);
    room.enqueue(nullptr, true);
    assert(room.waitingCount() == 0);
    std::cout << "PASS ClinicRoom empty room, configure, nullptr enqueue ignored\n";
}

void checkRoomQueueSelection() {
    Patient normal = makePatient("BN01", 1, 0, Priority::Normal);
    Patient pref = makePatient("BN02", 2, 0, Priority::Preferential);
    Patient emer = makePatient("BN03", 3, 0, Priority::Emergency);

    ClinicRoom on;
    on.configure(1, 1);
    on.enqueue(&normal, true);
    on.enqueue(&pref, true);
    on.enqueue(&emer, true);
    assert(on.normalQueueSize() == 1 && on.priorityQueueSize() == 2);
    assert(on.waitingCount() == 3);

    ClinicRoom off;
    off.configure(2, 1);
    off.enqueue(&normal, false);
    off.enqueue(&pref, false);
    off.enqueue(&emer, false);
    assert(off.normalQueueSize() == 3 && off.priorityQueueSize() == 0);
    // Tat uu tien: dung thu tu vao hang.
    assert(off.startNext(0, false) == &normal);
    std::cout << "PASS ClinicRoom priority on: level 0/1 to heap, 2 to FIFO; off: all FIFO\n";
}

void checkRoomStartFinish() {
    Patient p1 = makePatient("BN01", 1, 0, Priority::Normal, 5);
    Patient p2 = makePatient("BN02", 2, 1, Priority::Normal, 5);
    ClinicRoom room;
    room.configure(7, 2);
    room.enqueue(&p1, true);
    room.enqueue(&p2, true);

    assert(room.startNext(3, true) == &p1);
    assert(p1.status == PatientStatus::InService);
    assert(p1.assignedRoomId == 7 && p1.startTime == 3 && p1.finishTime == 8);
    assert(room.isBusy() && room.currentPatient() == &p1);
    // Dang khong tinh vao so cho.
    assert(room.waitingCount() == 1);
    // Phong ban khong goi them.
    assert(room.startNext(4, true) == nullptr && room.waitingCount() == 1);
    // Khong doi cau hinh khi dang ban.
    room.configure(99, 3);
    assert(room.id() == 7 && room.specialtyId() == 2);

    assert(room.finishIfDue(7) == nullptr); // Truoc gio xong.
    assert(room.finishIfDue(8) == &p1);     // Dung gio xong.
    assert(p1.status == PatientStatus::Completed);
    assert(!room.isBusy() && room.currentPatient() == nullptr);
    assert(room.finishIfDue(8) == nullptr); // Goi lan hai khong tra lai.

    assert(room.startNext(8, true) == &p2);
    assert(p2.startTime == 8 && p2.finishTime == 13);
    std::cout << "PASS ClinicRoom start/finish times, busy guard, finish once\n";
}

void checkRoomReset() {
    Patient p1 = makePatient("BN01", 1, 0, Priority::Normal);
    Patient p2 = makePatient("BN02", 2, 0, Priority::Emergency);
    Patient p3 = makePatient("BN03", 3, 0, Priority::Normal);
    ClinicRoom room;
    room.configure(1, 1);
    room.enqueue(&p1, true);
    room.enqueue(&p2, true);
    room.enqueue(&p3, true);
    assert(room.startNext(0, true) == &p2);
    room.reset();
    assert(!room.isBusy() && room.waitingCount() == 0);
    assert(room.normalQueueSize() == 0 && room.priorityQueueSize() == 0);
    // Reset khong huy hay sua Patient.
    assert(p1.input.id == "BN01" && p2.input.id == "BN02" && p3.input.id == "BN03");
    room.configure(5, 3);
    assert(room.id() == 5 && room.specialtyId() == 3);
    room.enqueue(&p1, false);
    assert(room.startNext(1, false) == &p1);
    std::cout << "PASS ClinicRoom reset clears queues/current, keeps Patient, reusable\n";
}

// Vi du 3 nguoi cua Leader, mo phong tay tung phut tren mot phong.
void runThreePatientExample(bool usePriority, int expectedWait[3]) {
    Patient p[3] = {
        makePatient("BN01", 1, 0, Priority::Normal, 5),
        makePatient("BN02", 2, 1, Priority::Normal, 5),
        makePatient("BN03", 3, 2, Priority::Emergency, 5),
    };
    for (Patient& x : p) x.status = PatientStatus::NotArrived;
    ClinicRoom room;
    room.configure(1, 1);
    int completed = 0;
    for (int minute = 0; completed < 3 && minute < 100; ++minute) {
        if (room.finishIfDue(minute) != nullptr) ++completed;
        for (Patient& x : p) {
            if (x.input.arrivalTime == minute) {
                x.status = PatientStatus::Waiting;
                room.enqueue(&x, usePriority);
            }
        }
        room.startNext(minute, usePriority);
    }
    assert(completed == 3);
    for (int i = 0; i < 3; ++i) {
        assert(p[i].status == PatientStatus::Completed);
        assert(waitingTime(p[i]) == expectedWait[i]);
    }
}

void checkLeaderExample() {
    int withPriority[3] = {0, 9, 3};
    int withoutPriority[3] = {0, 4, 8};
    runThreePatientExample(true, withPriority);
    runThreePatientExample(false, withoutPriority);
    std::cout << "PASS ClinicRoom leader example waits 0/9/3 (on) and 0/4/8 (off)\n";
}

#endif // TEST_A_QUEUE_ONLY

} // namespace

int main() {
    checkQueueEmpty();
    checkQueueSingleAndRefill();
    checkQueueFifoOrder();
    checkQueueClearKeepsPatients();
#ifndef TEST_A_QUEUE_ONLY
    checkRoomEmptyAndConfigure();
    checkRoomQueueSelection();
    checkRoomStartFinish();
    checkRoomReset();
    checkLeaderExample();
#else
    std::cout << "SKIP ClinicRoom (TEST_A_QUEUE_ONLY: can PriorityHeap that)\n";
#endif
    return 0;
}
