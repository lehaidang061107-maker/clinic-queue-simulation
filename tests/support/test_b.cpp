#include "Patient.h"
#include "PatientStore.h"
#include "PriorityHeap.h"
#include "Statistics.h"

#include <cassert>
#include <iostream>
#include <stdexcept>
#include <string>

using namespace clinic;

namespace {

PatientInput makeInput(const std::string& id, Priority prio, int arrival, int ticket) {
    return {id, "Patient " + id, 30, prio, 1, ticket, arrival, 5};
}

Patient makePatient(const std::string& id, Priority prio, int arrival, int ticket, int start = -1) {
    Patient p;
    p.input = makeInput(id, prio, arrival, ticket);
    if (start >= 0) {
        p.status = PatientStatus::Completed;
        p.assignedRoomId = 1;
        p.startTime = start;
        p.finishTime = start + 5;
    }
    return p;
}

void checkPriorityHeap() {
    PriorityHeap heap;
    assert(heap.isEmpty() && heap.size() == 0);
    assert(heap.peek() == nullptr && heap.pop() == nullptr);

    bool threwNull = false;
    try {
        heap.push(nullptr);
    } catch (const std::invalid_argument&) {
        threwNull = true;
    }
    assert(threwNull);

    // Uu tien: priority -> arrivalTime -> ticketNumber
    Patient bn01 = makePatient("BN01", Priority::Preferential, 1, 1);
    Patient bn02 = makePatient("BN02", Priority::Emergency, 5, 2);
    Patient bn03 = makePatient("BN03", Priority::Preferential, 1, 3);

    heap.push(&bn03);
    heap.push(&bn01);
    heap.push(&bn02);

    assert(heap.size() == 3);
    assert(heap.peek() == &bn02);
    assert(heap.pop() == &bn02);
    assert(heap.pop() == &bn01);
    assert(heap.pop() == &bn03);
    assert(heap.isEmpty() && heap.pop() == nullptr);

    // Vuot suc chua mac dinh 16 (grow) va clear roi tai su dung
    Patient pool[25];
    for (int i = 0; i < 25; ++i) {
        pool[i] = makePatient("P" + std::to_string(i), Priority::Normal, i, 25 - i);
        heap.push(&pool[i]);
    }
    assert(heap.size() == 25);
    assert(heap.peek() == &pool[0]);

    heap.clear();
    assert(heap.isEmpty() && heap.size() == 0 && heap.pop() == nullptr);

    heap.push(&bn01);
    assert(heap.size() == 1 && heap.pop() == &bn01);

    std::cout << "PASS PriorityHeap ordering, overflow guard, grow, clear and reuse\n";
}

void checkStatistics() {
    Statistics stats;

    bool threwInvalidRooms = false;
    try {
        stats.reset(-1);
    } catch (const std::invalid_argument&) {
        threwInvalidRooms = true;
    }
    assert(threwInvalidRooms);

    stats.reset(2);

    bool threwLow = false;
    try {
        (void)stats.byRoom(-1);
    } catch (const std::out_of_range&) {
        threwLow = true;
    }
    assert(threwLow);

    bool threwHigh = false;
    try {
        (void)stats.byRoom(2);
    } catch (const std::out_of_range&) {
        threwHigh = true;
    }
    assert(threwHigh);

    assert(stats.byPriority(Priority::Emergency).startedCount == 0);
    assert(stats.completedCount() == 0);

    // Khong ghi nhan benh nhan chua bat dau kham
    Patient notStarted = makePatient("WAIT", Priority::Normal, 0, 10, -1);
    stats.recordStart(notStarted);
    assert(stats.byPriority(Priority::Normal).startedCount == 0);

    // Hai benh nhan muc Preferential co thoi gian cho 4 va 8 phut
    Patient p1 = makePatient("BN01", Priority::Preferential, 2, 1, 6);  // wait = 4
    Patient p2 = makePatient("BN02", Priority::Preferential, 0, 2, 8);  // wait = 8
    stats.recordStart(p1);
    stats.recordStart(p2);
    stats.recordCompletion();
    stats.recordCompletion();

    const PriorityStats& pref = stats.byPriority(Priority::Preferential);
    assert(pref.startedCount == 2);
    assert(pref.totalWait == 12);
    assert(pref.maxWait == 8);
    assert(stats.completedCount() == 2);

    stats.observeQueues(0, 3, 4);
    stats.observeQueues(1, 2, 3);
    assert(stats.byRoom(0).roomId == 1 && stats.byRoom(0).maxWaiting == 3);
    assert(stats.byRoom(1).roomId == 2 && stats.byRoom(1).maxWaiting == 2);
    assert(stats.maxSystemWaiting() == 4);

    // Reset khong giu so lieu cu
    stats.reset(1);
    assert(stats.byPriority(Priority::Preferential).startedCount == 0);
    assert(stats.byPriority(Priority::Preferential).totalWait == 0);
    assert(stats.byPriority(Priority::Preferential).maxWait == 0);
    assert(stats.byRoom(0).maxWaiting == 0);
    assert(stats.maxSystemWaiting() == 0);
    assert(stats.completedCount() == 0);

    std::cout << "PASS Statistics priority aggregation, bounds check, queue peaks and reset\n";
}

void checkSortedWaitReport() {
    PatientStore emptyStore;
    int count = -1;
    const Patient** emptyReport = makeSortedWaitReport(emptyStore, count);
    assert(count == 0 && emptyReport == nullptr);

    PatientStore store;
    std::string error;
    assert(store.add(makeInput("BN01", Priority::Normal, 0, 1), error));       // start=2 -> wait=2
    assert(store.add(makeInput("BN02", Priority::Normal, 1, 2), error));       // start=9 -> wait=8
    assert(store.add(makeInput("BN03", Priority::Preferential, 2, 3), error)); // start=10 -> wait=8 (hoa BN02)
    assert(store.add(makeInput("BN04", Priority::Emergency, 3, 4), error));    // start=15 -> wait=12
    assert(store.add(makeInput("BN05", Priority::Normal, 4, 5), error));       // chua kham (start=-1)

    const_cast<Patient*>(store.findById("BN01"))->startTime = 2;
    const_cast<Patient*>(store.findById("BN02"))->startTime = 9;
    const_cast<Patient*>(store.findById("BN03"))->startTime = 10;
    const_cast<Patient*>(store.findById("BN04"))->startTime = 15;

    const Patient** report = makeSortedWaitReport(store, count);
    assert(count == 4 && report != nullptr);
    assert(report[0]->input.id == "BN04"); // wait 12
    assert(report[1]->input.id == "BN02"); // wait 8, ticket 2
    assert(report[2]->input.id == "BN03"); // wait 8, ticket 3
    assert(report[3]->input.id == "BN01"); // wait 2

    delete[] report;
    assert(store.size() == 5);

    std::cout << "PASS MergeSort wait report descending order, ticket tie-breaker and empty store\n";
}

} // namespace

int main() {
    checkPriorityHeap();
    checkStatistics();
    checkSortedWaitReport();
    return 0;
}
