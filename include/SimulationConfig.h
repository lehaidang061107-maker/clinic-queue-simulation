#ifndef CLINIC_SIMULATION_CONFIG_H
#define CLINIC_SIMULATION_CONFIG_H
#include <string>
namespace clinic {
struct SimulationConfig {
    int closingTime = 480; // Nhan benh trong [0, closingTime).
    int roomsPerSpecialty[3] = {1, 1, 1};
    bool usePriority = true;
};
struct GeneratorConfig {
    int patientCount = 30;
    int closingTime = 480;
    int minServiceDuration = 5;
    int maxServiceDuration = 15;
    unsigned int seed = 2026;
};
bool validateSimulationConfig(const SimulationConfig&, std::string& error);
bool validateGeneratorConfig(const GeneratorConfig&, std::string& error);
} // namespace clinic
#endif
