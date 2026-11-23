#include "Simulation.h"
#ifndef LEADER_ISOLATED_TEST
#include "DataGenerator.h"
#endif
#include <cassert>
#include <iostream>
#include <limits>
#include <stdexcept>

using namespace clinic;
namespace {
PatientInput input(const std::string& id, int ticket, int arrival = 0, bool emergency = false) {
    return {id, "Test Patient", 30, classifyPriority(30, emergency), 1, ticket, arrival, 5};
}
void add(PatientStore& store, const PatientInput& p) {
    std::string error;
    assert(store.add(p, error));
}
void finish(Simulation& sim) { int steps = 0; while (sim.step()) assert(++steps < 2000); assert(sim.isFinished()); }
void validators() {
    std::string error;
    for (int age : {0, 5, 6, 59, 60, 120}) {
        assert(classifyPriority(age, false) == (age < 6 || age >= 60 ? Priority::Preferential : Priority::Normal));
        assert(classifyPriority(age, true) == Priority::Emergency);
    }
    PatientInput p = input("P", 1);
    assert(validatePatientInput(p, 480, error) && error.empty());
    for (int age : {-1, 121}) { p.age = age; assert(!validatePatientInput(p, 480, error)); }
    p = input("P", 1);
    for (int specialty : {0, 4}) { p.specialtyId = specialty; assert(!validatePatientInput(p, 480, error)); }
    p = input("P", 1); p.priority = static_cast<Priority>(3); assert(!validatePatientInput(p, 480, error));
    p.priority = Priority::Preferential; assert(!validatePatientInput(p, 480, error));
    p = input("P", 0); assert(!validatePatientInput(p, 480, error));
    p = input("P", 1, 479); assert(validatePatientInput(p, 480, error));
    p.arrivalTime = 480; assert(!validatePatientInput(p, 480, error));
    p.arrivalTime = -1; assert(!validatePatientInput(p, 480, error));
    p = input("P", 1); p.serviceDuration = 0; assert(!validatePatientInput(p, 480, error));
    p.serviceDuration = std::numeric_limits<int>::max(); assert(!validatePatientInput(p, 480, error));
    p = input("|", 1); assert(!validatePatientInput(p, 480, error));
    p = input("P", 1); p.fullName = " \t"; assert(!validatePatientInput(p, 480, error));
    Patient patient{input("P", 1)};
    bool threw = false;
    try { waitingTime(patient); } catch (const std::logic_error&) { threw = true; }
    assert(threw);
    patient.startTime = 9; assert(waitingTime(patient) == 9);
    patient.status = PatientStatus::Completed; patient.assignedRoomId = 2; patient.finishTime = 14;
    resetPatientState(patient);
    assert(patient.status == PatientStatus::NotArrived && patient.startTime == -1 && patient.finishTime == -1 && patient.assignedRoomId == -1);
    assert(patient.input.id == "P");
    SimulationConfig config;
    assert(validateSimulationConfig(config, error));
    config.closingTime = 0; assert(!validateSimulationConfig(config, error));
    config.closingTime = std::numeric_limits<int>::max(); assert(!validateSimulationConfig(config, error));
    config = {}; config.roomsPerSpecialty[0] = 0; assert(!validateSimulationConfig(config, error));
    config.roomsPerSpecialty[0] = std::numeric_limits<int>::max(); assert(!validateSimulationConfig(config, error));
    GeneratorConfig generator;
    assert(validateGeneratorConfig(generator, error));
    generator.patientCount = 0; assert(validateGeneratorConfig(generator, error));
    generator.patientCount = -1; assert(!validateGeneratorConfig(generator, error));
    generator = {}; generator.maxServiceDuration = 4; assert(!validateGeneratorConfig(generator, error));
    generator = {}; generator.patientCount = std::numeric_limits<int>::max(); assert(!validateGeneratorConfig(generator, error));
    std::cout << "PASS validators, priority boundaries, reset, integer bounds\n";
}
void scenarios() {
    PatientStore store;
    add(store, input("BN03", 3, 2, true));
    add(store, input("BN01", 1));
    add(store, input("BN02", 2, 1));
    Simulation sim(store);
    assert(!sim.step() && !sim.isFinished());
    std::string error;
    for (int rooms : {1, 2}) for (bool priority : {true, false}) {
        SimulationConfig config;
        config.usePriority = priority;
        for (int& count : config.roomsPerSpecialty) count = rooms;
        assert(sim.initialize(config, error));
        assert(sim.lastProcessedMinute() == -1 && sim.roomCount() == rooms * 3);
        assert(sim.statistics().completedCount() == 0);
        assert(sim.step());
        assert(store.findById("BN01")->startTime == 0);
        SimulationConfig tooShort = config; tooShort.closingTime = 2;
        assert(!sim.initialize(tooShort, error));
        assert(sim.nextMinute() == 1 && store.findById("BN01")->startTime == 0);
        assert(sim.step() && sim.step());
        assert(sim.roomAt(0).currentPatient()->input.id == "BN01"); // No preemption.
        if (rooms == 1) assert(sim.statistics().maxSystemWaiting() == 2);
        while (sim.nextMinute() <= 15) assert(sim.step());
        assert(!sim.isFinished()); // Still accepting arrivals.
        finish(sim);
        const Patient& p1 = *store.findById("BN01");
        const Patient& p2 = *store.findById("BN02");
        const Patient& p3 = *store.findById("BN03");
        assert(waitingTime(p1) == 0);
        if (rooms == 1) {
            assert(p2.startTime == (priority ? 10 : 5));
            assert(p3.startTime == (priority ? 5 : 10));
            assert(waitingTime(p2) == (priority ? 9 : 4));
            assert(waitingTime(p3) == (priority ? 3 : 8));
        } else {
            assert(p2.assignedRoomId == 1 && p3.assignedRoomId == 2);
            assert(p2.startTime == 5 && p3.startTime == 2);
        }
        const auto& stats = sim.statistics();
        assert(stats.completedCount() == 3);
        assert(stats.byPriority(Priority::Normal).startedCount == 2);
        assert(stats.byPriority(Priority::Emergency).startedCount == 1);
        assert(stats.byPriority(Priority::Normal).totalWait == waitingTime(p2));
        assert(!sim.step() && stats.completedCount() == 3);
        assert(p3.input.priority == Priority::Emergency);
        std::cout << "PASS scenario rooms=" << rooms << " priority=" << priority
                  << " starts=" << p1.startTime << '/' << p2.startTime << '/' << p3.startTime
                  << " waits=" << waitingTime(p1) << '/' << waitingTime(p2) << '/' << waitingTime(p3) << '\n';
    }
}
void arrivalsAndLifecycle() {
    PatientStore store;
    add(store, input("later-ticket", 7));
    add(store, input("first-ticket", 2));
    add(store, input("emergency", 9, 0, true));
    PatientInput other = input("other", 10); other.specialtyId = 3; add(store, other);
    Simulation sim(store);
    SimulationConfig config; config.closingTime = 1; config.roomsPerSpecialty[0] = 2;
    std::string error;
    assert(sim.initialize(config, error) && sim.step());
    assert(store.findById("first-ticket")->assignedRoomId == 1);
    assert(store.findById("later-ticket")->assignedRoomId == 2);
    assert(store.findById("emergency")->assignedRoomId == 1);
    assert(store.findById("emergency")->startTime == 0); // All arrivals queued before any start.
    assert(store.findById("first-ticket")->status == PatientStatus::Waiting);
    assert(store.findById("other")->assignedRoomId == 4);
    assert(sim.statistics().maxSystemWaiting() == 4); // Includes pre-start observation.
    SimulationConfig invalid = config; invalid.closingTime = 0;
    assert(!sim.initialize(invalid, error) && sim.nextMinute() == 1);
    assert(store.findById("emergency")->startTime == 0);
    invalid = config; invalid.closingTime = 0;
    assert(!sim.initialize(invalid, error));
    bool threw = false;
    try { sim.roomAt(-1); } catch (const std::out_of_range&) { threw = true; }
    assert(threw);
    finish(sim);
    assert(sim.lastProcessedMinute() == 10 && sim.statistics().completedCount() == 4);
    // Reset while old queues/current still borrow patients.
    assert(sim.initialize(config, error) && sim.step());
    assert(sim.initialize(config, error));
    assert(store.findById("emergency")->status == PatientStatus::NotArrived);
    std::cout << "PASS simultaneous arrivals, room ties, specialties, late completion, lifecycle\n";
}
void admissionAndOverflow() {
    PatientStore store;
    Simulation sim(store);
    std::string error;
    assert(!sim.admit("A", "Name", 30, false, 1, 5, error));
    assert(sim.initialize({}, error));
    while (sim.nextMinute() < 479) assert(sim.step());
    assert(sim.admit("A", "Name", 30, false, 1, 5, error));
    const Patient* a = store.findById("A");
    assert(a->input.ticketNumber == 1 && a->input.arrivalTime == 479);
    assert(!sim.admit("A", "Name", 30, false, 1, 5, error));
    assert(!sim.admit("B", "Name", -1, false, 1, 5, error));
    assert(!sim.admit("B", "Name", 30, false, 4, 5, error));
    assert(store.size() == 1 && sim.step());
    assert(!sim.admit("B", "Name", 30, false, 1, 5, error));
    assert(!sim.isFinished()); finish(sim);
    assert(sim.lastProcessedMinute() == 484);
    PatientStore gaps;
    add(gaps, input("two", 2)); add(gaps, input("seven", 7));
    Simulation gapSim(gaps); assert(gapSim.initialize({}, error));
    assert(gapSim.admit("eight", "Name", 30, false, 1, 5, error));
    assert(gaps.findById("eight")->input.ticketNumber == 8);
    PatientStore full;
    add(full, input("max", std::numeric_limits<int>::max()));
    Simulation fullSim(full); assert(fullSim.initialize({}, error));
    assert(!fullSim.admit("X", "Name", 30, false, 1, 5, error) && full.size() == 1);
    PatientStore large;
    PatientInput p = input("large", 1); p.serviceDuration = std::numeric_limits<int>::max() - 480;
    add(large, p);
    Simulation largeSim(large); assert(largeSim.initialize({}, error));
    assert(!largeSim.admit("extra", "Name", 30, false, 1, 1, error));
    SimulationConfig config; config.closingTime = 481;
    assert(!largeSim.initialize(config, error));
    assert(largeSim.config().closingTime == 480);
    // Invalid existing input must be rejected without resetting an active scenario.
    PatientStore invalidStore; add(invalidStore, input("bad", 1, 480));
    Simulation invalidSim(invalidStore); assert(!invalidSim.initialize({}, error));
    std::cout << "PASS admission 479/480, ticket gaps/overflow, duration budget, invalid input\n";
}
void immediateAdmission() {
    std::string error;
    // Hoa phong: 2 phong Noi trong, 2 nguoi cung phut -> phong ma nho roi phong con lai.
    PatientStore store; Simulation sim(store);
    SimulationConfig two; two.roomsPerSpecialty[0] = 2;
    assert(sim.initialize(two, error));
    assert(sim.admit("T1", "Name", 30, false, 1, 5, error));
    assert(store.findById("T1")->assignedRoomId == 1);
    assert(store.findById("T1")->status == PatientStatus::Waiting);
    assert(sim.roomAt(0).waitingCount() == 1);
    assert(sim.admit("T2", "Name", 30, false, 1, 5, error));
    assert(store.findById("T2")->assignedRoomId == 2);
    assert(sim.step());
    assert(store.findById("T1")->startTime == 0 && store.findById("T2")->startTime == 0);
    // Nguoi dang kham khong tinh vao so cho: ca hai phong ban, hang deu rong -> phong 1.
    assert(sim.admit("T3", "Name", 30, false, 1, 5, error));
    assert(store.findById("T3")->assignedRoomId == 1 && store.findById("T3")->input.arrivalTime == 1);
    finish(sim);
    assert(sim.statistics().completedCount() == 3);

    // Cap cuu khi ban: goi ngay sau ca hien tai, truoc nguoi thuong dang cho.
    PatientStore busy; Simulation bsim(busy);
    assert(bsim.initialize({}, error));
    assert(bsim.admit("N1", "Name", 30, false, 1, 5, error));
    assert(bsim.admit("N2", "Name", 30, false, 1, 5, error));
    assert(bsim.step() && bsim.step()); // N1 kham 0..5, N2 cho FIFO.
    assert(bsim.admit("E1", "Name", 30, true, 1, 5, error));
    assert(busy.findById("E1")->input.priority == Priority::Emergency);
    assert(bsim.roomAt(0).priorityQueueSize() == 1 && bsim.roomAt(0).normalQueueSize() == 1);
    assert(bsim.roomAt(0).currentPatient() == busy.findById("N1")); // Khong ngat ca dang kham.
    finish(bsim);
    assert(busy.findById("E1")->startTime == 5 && busy.findById("N2")->startTime == 10);

    // Nguoi tu tep den cung phut van duoc xep truoc nguoi tiep nhan (thu tu arrival, ticket).
    PatientStore mixed; add(mixed, input("F1", 1, 1));
    Simulation msim(mixed); assert(msim.initialize(two, error) && msim.step());
    assert(msim.admit("M1", "Name", 30, false, 1, 5, error));
    assert(mixed.findById("F1")->assignedRoomId == 1 && mixed.findById("M1")->assignedRoomId == 2);
    std::cout << "PASS immediate room assignment on admit, room ties, emergency while busy\n";
}
}
#ifndef LEADER_ISOLATED_TEST
// ---- Giai doan 4: bat bien tren ca ngay, bo sinh seed 2026 (giong data/patients_day.txt) ----
bool before(const Patient& a, const Patient& b) {
    return a.input.arrivalTime < b.input.arrivalTime ||
           (a.input.arrivalTime == b.input.arrivalTime && a.input.ticketNumber < b.input.ticketNumber);
}
void generateDay(PatientStore& store, int count) {
    GeneratorConfig generator; generator.patientCount = count; generator.seed = 2026;
    std::string error;
    assert(generatePatients(generator, store, error));
    assert(store.size() == count);
}
struct RunInfo { int overtakes = 0; int mixedCalls = 0; };
// Chay het ngay; sau moi buoc kiem nguoi vua duoc goi so voi nguoi con cho cung phong.
// overtakes: nguoi duoc goi vuot qua mot nguoi den truoc (arrival, ticket) dang cho cung phong.
// mixedCalls: luc goi, phong co ca nguoi o heap lan nguoi o FIFO.
RunInfo runDay(Simulation& sim, PatientStore& store, int rooms, bool priority) {
    SimulationConfig config; config.usePriority = priority;
    for (int& n : config.roomsPerSpecialty) n = rooms;
    std::string error;
    assert(sim.initialize(config, error));
    RunInfo info;
    int steps = 0;
    while (sim.step()) {
        assert(++steps < 100000);
        const int minute = sim.lastProcessedMinute();
        for (int r = 0; r < sim.roomCount(); ++r) {
            const ClinicRoom& room = sim.roomAt(r);
            const Patient* called = room.currentPatient();
            if (!called || called->startTime != minute) continue;
            if (priority && called->input.priority != Priority::Normal && room.normalQueueSize() > 0)
                ++info.mixedCalls;
            for (const PatientStore::Node* node = store.head(); node; node = node->next) {
                const Patient& other = node->patient;
                if (other.status != PatientStatus::Waiting || other.assignedRoomId != room.id()) continue;
                if (before(other, *called)) ++info.overtakes;
                // Bat uu tien: khong goi muc thap hon khi muc cao hon dang cho cung phong.
                if (priority) assert(static_cast<int>(other.input.priority) >= static_cast<int>(called->input.priority));
            }
        }
    }
    assert(sim.isFinished());
    return info;
}
void checkDayInvariants(const Simulation& sim, const PatientStore& store, bool priority) {
    const int n = store.size();
    int started = 0;
    long long totalWait[kPriorityCount] = {0, 0, 0};
    int maxWait[kPriorityCount] = {0, 0, 0};
    int count[kPriorityCount] = {0, 0, 0};
    for (int level = 0; level < kPriorityCount; ++level)
        started += sim.statistics().byPriority(static_cast<Priority>(level)).startedCount;
    assert(started == n && sim.statistics().completedCount() == n);
    for (const PatientStore::Node* a = store.head(); a; a = a->next) {
        const Patient& p = a->patient;
        assert(p.status == PatientStatus::Completed);
        assert(p.startTime >= p.input.arrivalTime);
        assert(p.finishTime == p.startTime + p.input.serviceDuration);
        assert(p.assignedRoomId >= 1 && p.assignedRoomId <= sim.roomCount());
        assert(sim.roomAt(p.assignedRoomId - 1).specialtyId() == p.input.specialtyId);
        const int level = static_cast<int>(p.input.priority);
        const int wait = waitingTime(p);
        totalWait[level] += wait; ++count[level];
        if (wait > maxWait[level]) maxWait[level] = wait;
        for (const PatientStore::Node* b = a->next; b; b = b->next) {
            const Patient& q = b->patient;
            if (q.assignedRoomId != p.assignedRoomId) continue;
            assert(p.finishTime <= q.startTime || q.finishTime <= p.startTime); // Khong chong ca.
            if (!priority) assert((p.startTime < q.startTime) == before(p, q)); // FIFO dung thu tu.
        }
    }
    for (int level = 0; level < kPriorityCount; ++level) {
        const PriorityStats& stats = sim.statistics().byPriority(static_cast<Priority>(level));
        assert(stats.startedCount == count[level] && stats.totalWait == totalWait[level]);
        assert(stats.maxWait == maxWait[level]);
    }
    assert(sim.lastFinishMinute() == sim.lastProcessedMinute());
}
void fullDay() {
    PatientStore store; generateDay(store, 180);
    Simulation sim(store);
    for (int rooms = 1; rooms <= 2; ++rooms) {
        for (int priority = 1; priority >= 0; --priority) {
            runDay(sim, store, rooms, priority == 1);
            checkDayInvariants(sim, store, priority == 1);
        }
    }
    // Ngay rong: khong ai xong, phut ket thuc -1.
    PatientStore empty; Simulation emptySim(empty);
    std::string error; assert(emptySim.initialize({}, error)); finish(emptySim);
    assert(emptySim.lastFinishMinute() == -1 && emptySim.lastProcessedMinute() == 479);
    std::cout << "PASS full day 180 patients x 4 configs: counts, times, rooms, no overlap, order, stats\n";
}
// Neu khong co lan goi nao vuot nguoi den truoc thi lich bat uu tien trung lich FIFO:
// moi phut cung trang thai, chon phong chi phu thuoc so cho, nen ket qua trung tung nguoi.
void overtakeExplanation() {
    const int sizes[] = {120, 150, 180};
    for (int size : sizes) {
        for (int rooms = 1; rooms <= 2; ++rooms) {
            PatientStore store; generateDay(store, size);
            Simulation sim(store);
            const RunInfo on = runDay(sim, store, rooms, true);
            int startOn[400];
            int i = 0;
            for (const PatientStore::Node* node = store.head(); node; node = node->next) startOn[i++] = node->patient.startTime;
            runDay(sim, store, rooms, false);
            bool same = true;
            i = 0;
            for (const PatientStore::Node* node = store.head(); node; node = node->next)
                if (node->patient.startTime != startOn[i++]) same = false;
            std::cout << "INFO n=" << size << " rooms=" << rooms << " overtakes=" << on.overtakes
                      << " mixedCalls=" << on.mixedCalls << " sameSchedule=" << (same ? "yes" : "no") << '\n';
            assert(on.overtakes != 0 || same); // Khong vuot ai => phai trung lich.
            assert(!same || on.overtakes == 0); // Trung lich ma van co vuot => loi.
        }
    }
    std::cout << "PASS priority on/off identical schedule iff no overtakes\n";
}
#endif
#ifdef LEADER_ISOLATED_TEST
namespace clinic {
extern bool failNextEnqueueForLeaderTest;
extern bool failNextStatisticsResetForLeaderTest;
}
#endif
int main() {
    validators(); scenarios(); arrivalsAndLifecycle(); admissionAndOverflow(); immediateAdmission();
#ifndef LEADER_ISOLATED_TEST
    fullDay(); overtakeExplanation();
#endif
#ifdef LEADER_ISOLATED_TEST
    PatientStore store; add(store, input("P", 1));
    Simulation sim(store); std::string error;
    assert(sim.initialize({}, error) && sim.step());
    failNextStatisticsResetForLeaderTest = true;
    assert(!sim.initialize({}, error) && sim.nextMinute() == 1);
    assert(store.findById("P")->status == PatientStatus::InService);
    assert(sim.initialize({}, error));
    failNextEnqueueForLeaderTest = true;
    bool threw = false;
    try { sim.step(); } catch (const std::bad_alloc&) { threw = true; }
    assert(threw && !sim.step());
    assert(sim.initialize({}, error)); finish(sim);
    assert(sim.statistics().completedCount() == 1);
    std::cout << "PASS allocation failure propagation, disabled retry, recovery\n";
#endif
}
