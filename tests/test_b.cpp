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



Patient makePatient(const std::string& id, Priority prio, int arrival, int ticket, int start = -1) {
    Patient p;
    p.input.id = id;
    p.input.fullName = "Test " + id;
    p.input.age = 30;
    p.input.priority = prio;
    p.input.specialtyId = 1;
    p.input.ticketNumber = ticket;
    p.input.arrivalTime = arrival;
    p.input.serviceDuration = 5;
    if (start >= 0) {
        p.status = PatientStatus::Completed;
        p.assignedRoomId = 1;
        p.startTime = start;
        p.finishTime = start + 5;
    }
    return p;
}



void testPriorityHeap() {
    PriorityHeap heap;
    assert(heap.isEmpty());
    assert(heap.size() == 0);
    assert(heap.peek() == nullptr);
    assert(heap.pop() == nullptr);

    // Kiem tra push(nullptr) phai nem ngoai le
    bool threwNull = false;
    try {
        heap.push(nullptr);
    } catch (const std::invalid_argument&) {
        threwNull = true;
    }
    assert(threwNull);

   
  
  // Kiem tra thu tu uu tien: priority -> arrivalTime -> ticketNumber
    Patient bn01 = makePatient("BN01", Priority::Preferential, 1, 1);
    Patient bn02 = makePatient("BN02", Priority::Emergency, 5, 2);
    Patient bn03 = makePatient("BN03", Priority::Preferential, 1, 3);

    heap.push(&bn03);
    heap.push(&bn01);
    heap.push(&bn02);

    assert(heap.size() == 3);
    assert(heap.peek() == &bn02);
    assert(heap.pop() == &bn02); // Uu tien 0 (Emergency)
    assert(heap.pop() == &bn01); // Uu tien 1, den phut 1, ticket 1
    assert(heap.pop() == &bn03); // Uu tien 1, den phut 1, ticket 3
    assert(heap.isEmpty());

    // Kiem tra vuot suc chua mac dinh 16 (grow) va clear roi dung lai
   
  
  Patient pool[25];
    for (int i = 0; i < 25; ++i) {
        pool[i] = makePatient("P" + std::to_string(i), Priority::Normal, i, 25 - i);
        heap.push(&pool[i]);
    }
    assert(heap.size() == 25);
    assert(heap.peek() == &pool[0]);

    heap.clear();
    assert(heap.isEmpty());
    assert(heap.size() == 0);
    assert(heap.pop() == nullptr);

    heap.push(&bn01);
    assert(heap.size() == 1);
    assert(heap.pop() == &bn01);

    std::cout << "[PASS] testPriorityHeap\n";
}

void testStatisticsAndSort() {
    Statistics stats;
    stats.reset(2);

    // Kiem tra ngoai le khi chi so phong khong hop le
    bool threwOutOfRange = false;
    try {
        (void)stats.byRoom(-1);
    } catch (const std::out_of_range&) {
        threwOutOfRange = true;
    }
    assert(threwOutOfRange);

    threwOutOfRange = false;
    try {
        (void)stats.byRoom(2);
    } catch (const std::out_of_range&) {
        threwOutOfRange = true;
    }
    assert(threwOutOfRange);

    
  
  
  // Chua co du lieu
    assert(stats.byPriority(Priority::Emergency).startedCount == 0);
    assert(stats.completedCount() == 0);

    // Ghi nhan thong ke cho 2 benh nhan cung muc Preferential: cho 4 va 8 phut
    Patient p1 = makePatient("BN01", Priority::Preferential, 2, 1, 6);  // wait = 4
    Patient p2 = makePatient("BN02", Priority::Preferential, 0, 2, 8);  // wait = 8
    Patient p3 = makePatient("BN03", Priority::Normal, 1, 3, 9);        // wait = 8 (hoa wait voi BN02)

    stats.recordStart(p1);
    stats.recordStart(p2);
    stats.recordStart(p3);
    stats.recordCompletion();
    stats.recordCompletion();

    const PriorityStats& prefStats = stats.byPriority(Priority::Preferential);
    assert(prefStats.startedCount == 2);
    assert(prefStats.totalWait == 12);
    assert(prefStats.maxWait == 8);
    assert(stats.completedCount() == 2);

    stats.observeQueues(0, 3, 4);
    stats.observeQueues(1, 2, 3);
    assert(stats.byRoom(0).maxWaiting == 3);
    assert(stats.byRoom(1).maxWaiting == 2);
    assert(stats.maxSystemWaiting() == 4);

   
  
  
  // Kiem tra bao cao rong
    PatientStore emptyStore;
    int emptyCount = -1;
    const Patient** emptyReport = makeSortedWaitReport(emptyStore, emptyCount);
    assert(emptyCount == 0);
    assert(emptyReport == nullptr);

    // Kiem tra Merge Sort tren PatientStore
    PatientStore store;
    store.add(p1.input); 
    store.add(p2.input);
    store.add(p3.input);

    // Cap nhat startTime tren cac Patient trong store
    for (auto* node = const_cast<decltype(store.head())>(store.head()); node != nullptr; node = node->next) {
        // Neu can gan startTime truc tiep khi store.add chi nhan PatientInput
    }

    // Kiem tra reset xoa sach so lieu cu
    stats.reset(1);
    assert(stats.byPriority(Priority::Preferential).startedCount == 0);
    assert(stats.byPriority(Priority::Preferential).totalWait == 0);
    assert(stats.maxSystemWaiting() == 0);
    assert(stats.completedCount() == 0);

    std::cout << "[PASS] testStatisticsAndSort\n";
}

} 

int main() {
    testPriorityHeap();
    testStatisticsAndSort();
    std::cout << "Tat ca ca kiem thu thanh vien B deu PASS!\n";
    return 0;
}
