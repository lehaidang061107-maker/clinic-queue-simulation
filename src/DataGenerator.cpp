#include "DataGenerator.h"

#include "Patient.h"

#include <cstdint>
#include <limits>
#include <memory>
#include <new>
#include <random>
#include <utility>

namespace clinic {
namespace {

struct GeneratedPatient {
    PatientInput input;
    int genIndex = 0;
};

std::uint64_t boundedRandom(std::mt19937& engine, std::uint64_t bound) {
    const std::uint64_t range = std::uint64_t{1} << 32;
    const std::uint64_t limit = range - range % bound;
    for (;;) {
        const std::uint64_t value = engine();
        if (value < limit) return value % bound;
    }
}

bool comesBefore(const GeneratedPatient& lhs, const GeneratedPatient& rhs) {
    if (lhs.input.arrivalTime != rhs.input.arrivalTime)
        return lhs.input.arrivalTime < rhs.input.arrivalTime;
    return lhs.genIndex < rhs.genIndex;
}

void mergeSort(GeneratedPatient* values, GeneratedPatient* temporary,
               int first, int last) {
    if (last - first < 2) return;
    const int middle = first + (last - first) / 2;
    mergeSort(values, temporary, first, middle);
    mergeSort(values, temporary, middle, last);

    int left = first;
    int right = middle;
    int output = first;
    while (left < middle && right < last) {
        if (comesBefore(values[right], values[left]))
            temporary[output++] = std::move(values[right++]);
        else
            temporary[output++] = std::move(values[left++]);
    }
    while (left < middle) temporary[output++] = std::move(values[left++]);
    while (right < last) temporary[output++] = std::move(values[right++]);
    for (int index = first; index < last; ++index)
        values[index] = std::move(temporary[index]);
}

} // namespace

bool generatePatients(const GeneratorConfig& config, PatientStore& store,
                      std::string& error) {
    error.clear();
    if (!store.isEmpty()) {
        error = "Chi co the sinh du lieu vao kho rong.";
        return false;
    }
    if (!validateGeneratorConfig(config, error)) return false;
    if (config.patientCount == 0) return true;

    try {
        const int count = config.patientCount;
        std::unique_ptr<GeneratedPatient[]> patients(new GeneratedPatient[count]);
        std::unique_ptr<GeneratedPatient[]> temporary(new GeneratedPatient[count]);
        std::mt19937 engine(config.seed);
        const std::uint64_t durationRange =
            static_cast<std::uint64_t>(config.maxServiceDuration) -
            static_cast<std::uint64_t>(config.minServiceDuration) + 1;

        for (int index = 0; index < count; ++index) {
            const int age = static_cast<int>(boundedRandom(engine, 121));
            const bool emergency = boundedRandom(engine, 10) == 0;
            PatientInput& input = patients[index].input;
            input.id = "BN" + std::to_string(index + 1);
            input.fullName = "Benh nhan " + std::to_string(index + 1);
            input.age = age;
            input.priority = classifyPriority(age, emergency);
            input.specialtyId = 1 + static_cast<int>(boundedRandom(engine, kSpecialtyCount));
            input.ticketNumber = 0;
            input.arrivalTime =
                static_cast<int>(boundedRandom(engine, static_cast<std::uint64_t>(config.closingTime)));
            input.serviceDuration = config.minServiceDuration +
                static_cast<int>(boundedRandom(engine, durationRange));
            patients[index].genIndex = index;
        }

        mergeSort(patients.get(), temporary.get(), 0, count);
        for (int index = 0; index < count; ++index) {
            PatientInput& input = patients[index].input;
            input.ticketNumber = index + 1;
            if (!validatePatientInput(input, config.closingTime, error) ||
                !store.add(input, error)) {
                store.clear();
                return false;
            }
        }
        return true;
    } catch (const std::bad_alloc&) {
        store.clear();
        error = "Khong du bo nho de sinh du lieu.";
        return false;
    }
}

} // namespace clinic
