#ifndef CLINIC_FILE_IO_H
#define CLINIC_FILE_IO_H
#include <iosfwd>
#include "PatientStore.h"
#include "Simulation.h"
namespace clinic {
struct LoadResult {
    bool opened = false;
    bool headerValid = false;
    int accepted = 0;
    int rejected = 0;
};
// Chi nap vao kho rong, truoc initialize. Bo dong sai, ghi ly do va so dong.
LoadResult loadPatients(const std::string& path, PatientStore& store,
                        int closingTime, std::ostream& diagnostics);
// Ghi dung dinh dang input, khong ghi cac truong trang thai phien.
bool savePatients(const std::string& path, const PatientStore&, std::string& error);
bool saveRunResults(const std::string& path, const PatientStore&,
                    const Simulation&, std::string& error);
} // namespace clinic
#endif
