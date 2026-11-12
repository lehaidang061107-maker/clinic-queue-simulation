#ifndef CLINIC_PATIENT_H
#define CLINIC_PATIENT_H
#include <string>
namespace clinic {
constexpr int kSpecialtyCount = 3;
constexpr int kPriorityCount = 3;
enum class Priority : int { Emergency = 0, Preferential = 1, Normal = 2 };
enum class PatientStatus { NotArrived, Waiting, InService, Completed };
// Du lieu dau vao bat bien trong mot lan mo phong.
struct PatientInput {
    std::string id;
    std::string fullName;
    int age = 0;
    Priority priority = Priority::Normal;
    int specialtyId = 1; // 1: Noi, 2: Nhi, 3: Ngoai.
    int ticketNumber = 0; // > 0 khi da tiep nhan.
    int arrivalTime = 0;
    int serviceDuration = 5;
};
struct Patient {
    PatientInput input;
    PatientStatus status = PatientStatus::NotArrived;
    int assignedRoomId = -1;
    int startTime = -1;
    int finishTime = -1;
};
Priority classifyPriority(int age, bool emergency);
bool validatePatientInput(const PatientInput& input, int closingTime,
                          std::string& error);
void resetPatientState(Patient& patient);
// Chi goi sau khi startTime >= 0.
int waitingTime(const Patient& patient);
} // namespace clinic
#endif
