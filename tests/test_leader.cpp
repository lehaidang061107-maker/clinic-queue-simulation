#include "Simulation.h"
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
#ifdef LEADER_ISOLATED_TEST
namespace clinic {
extern bool failNextEnqueueForLeaderTest;
extern bool failNextStatisticsResetForLeaderTest;
}
#endif
int main() {
    validators(); scenarios(); arrivalsAndLifecycle(); admissionAndOverflow(); immediateAdmission();
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
