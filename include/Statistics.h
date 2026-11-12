#ifndef CLINIC_STATISTICS_H
#define CLINIC_STATISTICS_H
#include "PatientStore.h"
namespace clinic {
struct PriorityStats {
    int startedCount = 0;
    long long totalWait = 0;
    int maxWait = 0;
    // startedCount=0: UI in "Chua co du lieu", khong chia cho 0.
};
struct RoomStats { int roomId = -1; int maxWaiting = 0; };
class Statistics {
public:
    Statistics();
    ~Statistics();
    Statistics(const Statistics&) = delete;
    Statistics& operator=(const Statistics&) = delete;
    void reset(int roomCount); // Khoi tao roomId theo index+1.
    void recordStart(const Patient& patient); // Dung 1 lan/nguoi/kich ban.
    void recordCompletion();
    void observeQueues(int roomIndex, int roomWaiting, int systemWaiting);
    const PriorityStats& byPriority(Priority priority) const;
    const RoomStats& byRoom(int roomIndex) const;
    int maxSystemWaiting() const;
    int completedCount() const;
private:
    PriorityStats priority_[kPriorityCount];
    RoomStats* rooms_;
    int roomCount_;
    int maxSystemWaiting_;
    int completedCount_;
};
// Tao mang con tro toi nguoi da bat dau kham, sap xep Merge Sort tu cai:
// wait giam dan, hoa thi ticket tang dan. Caller delete[] mang, khong xoa Patient.
const Patient** makeSortedWaitReport(const PatientStore&, int& count);
} // namespace clinic
#endif
