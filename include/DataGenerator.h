#ifndef CLINIC_DATA_GENERATOR_H
#define CLINIC_DATA_GENERATOR_H
#include "SimulationConfig.h"
#include "PatientStore.h"
namespace clinic {
// Store phai rong. Sinh input -> sap xep (arrival,thu tu sinh) -> cap ticket.
// Seed va cau hinh giong nhau phai tao cung tap input tren hai he dieu han.
bool generatePatients(const GeneratorConfig&, PatientStore&, std::string& error);
} // namespace clinic
#endif
