#include "DataGenerator.h"
#include "FileIO.h"
#include "Patient.h"
#include "PatientStore.h"

#include <cassert>
#include <cstdio>
#include <fstream>
#include <iostream>
#include <limits>
#include <sstream>
#include <stdexcept>
#include <string>

using namespace clinic;

namespace {

const char* const kHeader =
    "id|fullName|age|priority|specialtyId|ticketNumber|arrivalTime|serviceDuration";
const char* const kInputPath = "build/test_c_input.txt";
const char* const kSavedPath = "build/test_c_saved.txt";
const char* const kBadHeaderPath = "build/test_c_bad_header.txt";
const char* const kMissingPath = "build/test_c_missing.txt";
const char* const kGeneratedAPath = "build/test_c_generated_a.txt";
const char* const kGeneratedBPath = "build/test_c_generated_b.txt";

PatientInput patient(const std::string& id, int ticket, int arrival) {
    return {id, "Patient " + id, 30, Priority::Normal, 1, ticket, arrival, 5};
}

void checkStore() {
    PatientStore store;
    std::string error;
    assert(store.isEmpty() && store.size() == 0 && store.head() == nullptr);
    assert(store.add(patient("late", 7, 5), error));
    const Patient* stable = store.findById("late");
    assert(store.add(patient("first", 2, 1), error));
    assert(store.add(patient("same-time-high", 9, 1), error));
    assert(store.add(patient("same-time-low", 3, 1), error));
    assert(store.size() == 4 && !store.isEmpty());
    const PatientStore::Node* node = store.head();
    assert(node->patient.input.id == "first");
    node = node->next;
    assert(node->patient.input.id == "same-time-low");
    node = node->next;
    assert(node->patient.input.id == "same-time-high");
    node = node->next;
    assert(node->patient.input.id == "late");
    assert(store.findById("late") == stable);
    assert(store.findById("missing") == nullptr);
    assert(store.containsTicket(9) && !store.containsTicket(8));
    assert(store.nextTicketNumber() == 10);

    assert(!store.add(patient("late", 10, 0), error));
    assert(!error.empty() && store.size() == 4);
    assert(!store.add(patient("duplicate-ticket", 7, 0), error));
    assert(!error.empty() && store.size() == 4);

    PatientStore nonContiguous;
    assert(nonContiguous.add(patient("two", 2, 0), error));
    assert(nonContiguous.add(patient("seven", 7, 0), error));
    assert(nonContiguous.nextTicketNumber() == 8);

    PatientStore exhausted;
    assert(exhausted.add(patient("max", std::numeric_limits<int>::max(), 0), error));
    bool overflowReported = false;
    try {
        exhausted.nextTicketNumber();
    } catch (const std::overflow_error&) {
        overflowReported = true;
    }
    assert(overflowReported);

    store.resetStates();
    assert(store.findById("late")->status == PatientStatus::NotArrived);
    store.clear();
    assert(store.isEmpty() && store.size() == 0 && store.head() == nullptr);
    store.clear();
    assert(store.add(patient("reused", 1, 0), error));
    assert(store.size() == 1);
    std::cout << "PASS PatientStore ordering, uniqueness, search, tickets, reset and reuse\n";
}

void writeFile(const char* path, const std::string& contents) {
    std::ofstream output(path, std::ios::binary | std::ios::trunc);
    assert(output.is_open());
    output << contents;
    output.close();
    assert(static_cast<bool>(output));
}

std::string readFile(const char* path) {
    std::ifstream input(path, std::ios::binary);
    assert(input.is_open());
    std::ostringstream contents;
    contents << input.rdbuf();
    return contents.str();
}

void checkFileIO() {
    std::string contents = std::string(kHeader) + "\r\n"
        "A|Alice|30|2|1|2|3|5\r\n"
        "\r\n"
        "BAD|Age|abc|2|1|3|4|5\r\n"
        "NEG|Negative age|-1|2|1|12|4|5\r\n"
        "BADPR|Mismatched priority|65|2|1|13|4|5\r\n"
        "OVR|Overflow age|999999999999999999999|2|1|14|4|5\r\n"
        "B|Bob|65|1|2|7|1|10\r\n"
        "C|Duplicate ticket|40|2|1|7|2|5\r\n"
        "A|Duplicate id|30|2|1|8|2|5\r\n"
        "D|Bad specialty|30|2|4|9|2|5\r\n"
        "E|Late|30|2|1|10|480|5\r\n"
        "F|Missing field|30|2|1|11|4\r\n";
    writeFile(kInputPath, contents);

    PatientStore store;
    std::ostringstream diagnostics;
    const LoadResult loaded = loadPatients(kInputPath, store, 480, diagnostics);
    assert(loaded.opened && loaded.headerValid);
    assert(loaded.accepted == 2 && loaded.rejected == 9);
    assert(store.size() == 2 && store.findById("A") && store.findById("B"));
    assert(store.nextTicketNumber() == 8);
    assert(diagnostics.str().find("Dong 4:") != std::string::npos);
    assert(diagnostics.str().find("Dong 13:") != std::string::npos);

    std::string error;
    assert(savePatients(kSavedPath, store, error) && error.empty());
    PatientStore restored;
    std::ostringstream restoreDiagnostics;
    const LoadResult reloaded = loadPatients(kSavedPath, restored, 480, restoreDiagnostics);
    assert(reloaded.opened && reloaded.headerValid && reloaded.accepted == 2);
    assert(restored.findById("A") && restored.findById("B"));
    assert(restored.findById("A")->input.arrivalTime == 3);
    assert(restored.findById("B")->input.ticketNumber == 7);

    writeFile(kBadHeaderPath, "id|wrong-header\nA|Alice\n");
    PatientStore badHeaderStore;
    std::ostringstream badHeaderDiagnostics;
    const LoadResult badHeader =
        loadPatients(kBadHeaderPath, badHeaderStore, 480, badHeaderDiagnostics);
    assert(badHeader.opened && !badHeader.headerValid && badHeaderStore.isEmpty());

    PatientStore missingStore;
    std::ostringstream missingDiagnostics;
    const LoadResult missing =
        loadPatients(kMissingPath, missingStore, 480, missingDiagnostics);
    assert(!missing.opened && !missing.headerValid && missingStore.isEmpty());

    PatientStore nonEmpty;
    assert(nonEmpty.add(patient("existing", 1, 0), error));
    std::ostringstream nonEmptyDiagnostics;
    const LoadResult refused = loadPatients(kInputPath, nonEmpty, 480, nonEmptyDiagnostics);
    assert(!refused.opened && nonEmpty.size() == 1);

    assert(!savePatients("build/no_such_directory/output.txt", store, error));
    assert(!error.empty());
    std::remove(kInputPath);
    std::remove(kSavedPath);
    std::remove(kBadHeaderPath);
    std::cout << "PASS FileIO CRLF, bad-line isolation, malformed header, missing/unwritable paths and round trip\n";
}

void checkGenerator() {
    GeneratorConfig config;
    config.patientCount = 100;
    config.closingTime = 31;
    config.minServiceDuration = 5;
    config.maxServiceDuration = 15;
    config.seed = 987654321U;

    PatientStore first;
    PatientStore second;
    std::string error;
    assert(generatePatients(config, first, error) && error.empty());
    assert(generatePatients(config, second, error) && error.empty());
    assert(first.size() == config.patientCount && second.size() == config.patientCount);
    assert(first.nextTicketNumber() == config.patientCount + 1);

    const PatientStore::Node* node = first.head();
    int expectedTicket = 1;
    int previousArrival = -1;
    for (; node; node = node->next, ++expectedTicket) {
        const PatientInput& input = node->patient.input;
        assert(input.ticketNumber == expectedTicket);
        assert(input.arrivalTime >= previousArrival && input.arrivalTime < config.closingTime);
        assert(input.serviceDuration >= config.minServiceDuration &&
               input.serviceDuration <= config.maxServiceDuration);
        assert(input.age >= 0 && input.age <= 120);
        assert(input.specialtyId >= 1 && input.specialtyId <= kSpecialtyCount);
        assert(input.priority == classifyPriority(input.age,
               input.priority == Priority::Emergency));
        previousArrival = input.arrivalTime;
    }

    assert(savePatients(kGeneratedAPath, first, error));
    assert(savePatients(kGeneratedBPath, second, error));
    assert(readFile(kGeneratedAPath) == readFile(kGeneratedBPath));

    PatientStore occupied;
    assert(occupied.add(patient("existing", 1, 0), error));
    assert(!generatePatients(config, occupied, error) && occupied.size() == 1);
    GeneratorConfig invalid = config;
    invalid.maxServiceDuration = 0;
    PatientStore invalidStore;
    assert(!generatePatients(invalid, invalidStore, error) && invalidStore.isEmpty());

    std::remove(kGeneratedAPath);
    std::remove(kGeneratedBPath);
    std::cout << "PASS DataGenerator deterministic seed, ordering, ticket sequence and bounds\n";
}

void checkProjectDataFiles() {
    PatientStore sample;
    std::ostringstream sampleDiagnostics;
    const LoadResult sampleResult =
        loadPatients("data/patients_sample.txt", sample, 480, sampleDiagnostics);
    assert(sampleResult.opened && sampleResult.headerValid);
    assert(sampleResult.accepted == 30 && sampleResult.rejected == 0);

    bool specialties[4] = {};
    bool priorities[3] = {};
    bool requiredAges[6] = {};
    int arrivalsAtZero = 0;
    bool arrival479 = false;
    for (const PatientStore::Node* node = sample.head(); node; node = node->next) {
        const PatientInput& input = node->patient.input;
        specialties[input.specialtyId] = true;
        priorities[static_cast<int>(input.priority)] = true;
        if (input.age == 0) requiredAges[0] = true;
        if (input.age == 5) requiredAges[1] = true;
        if (input.age == 6) requiredAges[2] = true;
        if (input.age == 59) requiredAges[3] = true;
        if (input.age == 60) requiredAges[4] = true;
        if (input.age == 120) requiredAges[5] = true;
        if (input.arrivalTime == 0) ++arrivalsAtZero;
        if (input.arrivalTime == 479) arrival479 = true;
        assert(input.serviceDuration >= 5 && input.serviceDuration <= 15);
    }
    for (int specialty = 1; specialty <= 3; ++specialty) assert(specialties[specialty]);
    for (int priority = 0; priority < 3; ++priority) assert(priorities[priority]);
    for (bool agePresent : requiredAges) assert(agePresent);
    assert(arrivalsAtZero >= 3 && arrival479);

    PatientStore mixed;
    std::ostringstream mixedDiagnostics;
    const LoadResult mixedResult =
        loadPatients("tests/data/invalid_mix.txt", mixed, 480, mixedDiagnostics);
    assert(mixedResult.opened && mixedResult.headerValid);
    assert(mixedResult.accepted == 2 && mixedResult.rejected == 6);
    assert(mixed.size() == 2 && mixed.findById("OK01") && mixed.findById("OK02"));
    assert(mixedDiagnostics.str().find("Dong 3:") != std::string::npos);
    assert(mixedDiagnostics.str().find("Dong 8:") != std::string::npos);

    std::cout << "PASS project sample coverage and mixed valid/invalid input data\n";
}

} // namespace

int main() {
    checkStore();
    checkFileIO();
    checkGenerator();
    checkProjectDataFiles();
    return 0;
}
