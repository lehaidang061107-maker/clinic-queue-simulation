#ifndef CLINIC_PATIENT_STORE_H
#define CLINIC_PATIENT_STORE_H
#include "Patient.h"
namespace clinic {
// Chi PatientStore so huu Patient; dia chi Patient on dinh den khi clear().
class PatientStore {
public:
    struct Node { Patient patient; Node* next; };
    PatientStore();
    ~PatientStore();
    PatientStore(const PatientStore&) = delete;
    PatientStore& operator=(const PatientStore&) = delete;
    // Input da validate; van kiem tra trung id/ticket va thu tu (arrival,ticket).
    // Loi thi khong thay doi kho. Node duoc chen theo (arrival,ticket) tang dan.
    bool add(const PatientInput&, std::string& error);
    const Patient* findById(const std::string& id) const;
    bool containsTicket(int ticketNumber) const;
    int nextTicketNumber() const; // max ticket + 1; can kiem tra tran.
    int size() const;
    bool isEmpty() const;
    const Node* head() const; // Chi doc cho UI, FileIO va thong ke.
    void resetStates();
    void clear(); // Chi khi khong con hang doi/phong tham chieu.
private:
    Node* head_;
    int size_;
    friend class Simulation; // Simulation duoc duyet va cap nhat trang thai.
};
} // namespace clinic
#endif
