#ifndef CLINIC_SIMULATION_H
#define CLINIC_SIMULATION_H
#include "ClinicRoom.h"
#include "PatientStore.h"
#include "SimulationConfig.h"
#include "Statistics.h"
namespace clinic {
class Simulation {
public:
    explicit Simulation(PatientStore& store); // Store phai song lau hon Simulation.
    ~Simulation();
    Simulation(const Simulation&) = delete;
    Simulation& operator=(const Simulation&) = delete;
    // Validate config va toan bo input truoc khi reset/tao phong.
    bool initialize(const SimulationConfig&, std::string& error);
    // Tiep nhan thu cong chi truoc khi xu ly phut dau tien hoac giua cac buoc.
    // Ham tu cap ticket va arrivalTime=nextMinute(); validate truoc khi add.
    bool admit(const std::string& id, const std::string& name, int age,
               bool emergency, int specialtyId, int duration, std::string& error);
    // Xu ly tron phut nextMinute: finish -> arrivals -> start -> snapshot.
    // true: da xu ly 1 phut; false: chua initialize hoac da ket thuc.
    bool step();
    bool isFinished() const; // Da dong tiep nhan va khong con nguoi chua den/cho/dang kham.
    int nextMinute() const;
    int lastProcessedMinute() const; // -1 truoc buoc dau.
    int roomCount() const;
    const ClinicRoom& roomAt(int index) const;
    const Statistics& statistics() const;
    const SimulationConfig& config() const;
private:
    PatientStore& store_;
    SimulationConfig config_;
    ClinicRoom* rooms_;
    int roomCount_;
    int nextMinute_;
    bool initialized_;
    Statistics statistics_;
    int selectRoom(int specialtyId) const;
    void observeAllQueues();
};
} // namespace clinic
#endif
