#include "ConsoleUI.h"

#include "DataGenerator.h"
#include "FileIO.h"
#include "Statistics.h"

#include <charconv>
#include <cctype>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <limits>
#include <sstream>
#include <stdexcept>
#include <string>
#include <system_error>

namespace clinic {
namespace {

const char* specialtyName(int specialtyId) {
    switch (specialtyId) {
    case 1: return "Noi";
    case 2: return "Nhi";
    case 3: return "Ngoai";
    default: return "Khong ro";
    }
}

std::string trim(const std::string& value) {
    const std::size_t first = value.find_first_not_of(" \t\r\n");
    if (first == std::string::npos) return std::string();
    const std::size_t last = value.find_last_not_of(" \t\r\n");
    return value.substr(first, last - first + 1);
}

bool readLine(std::istream& in, std::ostream& out, const std::string& prompt,
              std::string& line) {
    out << prompt;
    out.flush();
    return static_cast<bool>(std::getline(in, line));
}

bool readInt(std::istream& in, std::ostream& out, const std::string& prompt,
             int minimum, int maximum, bool hasDefault, int defaultValue,
             int& value) {
    for (;;) {
        std::string line;
        if (!readLine(in, out, prompt, line)) return false;
        line = trim(line);
        if (line.empty() && hasDefault) {
            value = defaultValue;
            return true;
        }
        int parsed = 0;
        const char* first = line.data();
        const char* last = first + line.size();
        const std::from_chars_result conversion = std::from_chars(first, last, parsed);
        if (!line.empty() && conversion.ec == std::errc() && conversion.ptr == last &&
            parsed >= minimum && parsed <= maximum) {
            value = parsed;
            return true;
        }
        out << "Gia tri phai la so nguyen trong [" << minimum << ", " << maximum
            << "]. Vui long nhap lai.\n";
    }
}

bool readUnsigned(std::istream& in, std::ostream& out, const std::string& prompt,
                  unsigned int& value) {
    for (;;) {
        std::string line;
        if (!readLine(in, out, prompt, line)) return false;
        line = trim(line);
        unsigned int parsed = 0;
        const char* first = line.data();
        const char* last = first + line.size();
        const std::from_chars_result conversion = std::from_chars(first, last, parsed);
        if (!line.empty() && conversion.ec == std::errc() && conversion.ptr == last) {
            value = parsed;
            return true;
        }
        out << "Gia tri phai la so nguyen khong am trong mien unsigned int.\n";
    }
}

bool readYesNo(std::istream& in, std::ostream& out, const std::string& prompt,
               bool defaultValue, bool& value) {
    for (;;) {
        std::string line;
        if (!readLine(in, out, prompt, line)) return false;
        line = trim(line);
        if (line.empty()) {
            value = defaultValue;
            return true;
        }
        if (line == "1" || line == "y" || line == "Y" ||
            line == "yes" || line == "YES") {
            value = true;
            return true;
        }
        if (line == "0" || line == "n" || line == "N" ||
            line == "no" || line == "NO") {
            value = false;
            return true;
        }
        for (char& character : line)
            character = static_cast<char>(std::tolower(static_cast<unsigned char>(character)));
        if (line == "yes" || line == "y") {
            value = true;
            return true;
        }
        if (line == "no" || line == "n") {
            value = false;
            return true;
        }
        out << "Nhap y/yes/1 hoac n/no/0.\n";
    }
}

bool readPath(std::istream& in, std::ostream& out, const std::string& prompt,
              const std::string& defaultPath, std::string& path) {
    if (!readLine(in, out, prompt + " [" + defaultPath + "]: ", path)) return false;
    path = trim(path);
    if (path.empty()) path = defaultPath;
    return true;
}

bool promptConfiguration(std::istream& in, std::ostream& out,
                         const SimulationConfig& current, SimulationConfig& config) {
    config = current;
    int number = 0;
    if (!readInt(in, out, "Phut dong tiep nhan: ", 1,
                 std::numeric_limits<int>::max() - 1, true,
                 current.closingTime, number)) return false;
    config.closingTime = number;
    for (int specialty = 0; specialty < kSpecialtyCount; ++specialty) {
        const std::string prompt = "So phong khoa " + std::to_string(specialty + 1) + ": ";
        if (!readInt(in, out, prompt, 1, std::numeric_limits<int>::max(), true,
                     current.roomsPerSpecialty[specialty], number)) return false;
        config.roomsPerSpecialty[specialty] = number;
    }
    if (!readYesNo(in, out, "Bat uu tien? (y/n): ", current.usePriority,
                   config.usePriority)) return false;
    std::string error;
    if (!validateSimulationConfig(config, error)) {
        out << "Cau hinh khong hop le: " << error << '\n';
        return false;
    }
    return true;
}

bool saveInput(std::istream& in, std::ostream& out, const PatientStore& store,
               const std::string& defaultPath, bool retryUntilSaved) {
    for (;;) {
        std::string path;
        if (!readPath(in, out, "Duong dan tep dau vao", defaultPath, path)) return false;
        std::string error;
        if (savePatients(path, store, error)) {
            out << "Da luu " << store.size() << " benh nhan vao " << path << ".\n";
            return true;
        }
        out << "Luu that bai: " << error << '\n';
        if (!retryUntilSaved) return false;
        bool retry = false;
        if (!readYesNo(in, out, "Thu lai? (y/n): ", true, retry)) return false;
        if (!retry) return false;
    }
}

void printPriorityStatistics(const Statistics& statistics, std::ostream& out) {
    for (int level = 0; level < kPriorityCount; ++level) {
        const PriorityStats& values = statistics.byPriority(static_cast<Priority>(level));
        out << "Muc " << level << ": ";
        if (values.startedCount == 0) {
            out << "Chua co du lieu\n";
            continue;
        }
        out << "bat dau=" << values.startedCount
            << ", cho TB=" << static_cast<double>(values.totalWait) / values.startedCount
            << ", cho max=" << values.maxWait << '\n';
    }
}

struct ComparisonRow {
    std::string name;
    PriorityStats byPriority[kPriorityCount];
    bool hasAverageWait = false;
    double averageWait = 0.0;
    int maxSystemWaiting = 0;
    int finishMinute = -1;
    int started = 0;
    int completed = 0;
};

void captureComparisonRow(const Simulation& simulation, const std::string& name,
                          ComparisonRow& row) {
    row.name = name;
    const Statistics& statistics = simulation.statistics();
    for (int level = 0; level < kPriorityCount; ++level) {
        row.byPriority[level] = statistics.byPriority(static_cast<Priority>(level));
    }
    row.hasAverageWait = statistics.averageWait(row.averageWait);
    row.maxSystemWaiting = statistics.maxSystemWaiting();
    row.finishMinute = simulation.lastFinishMinute();
    row.started = 0;
    for (int level = 0; level < kPriorityCount; ++level)
        row.started += row.byPriority[level].startedCount;
    row.completed = statistics.completedCount();
}

bool runComparisonScenario(Simulation& simulation, const SimulationConfig& config,
                           const std::string& name, ComparisonRow& row,
                           std::ostream& out) {
    std::string error;
    if (!simulation.initialize(config, error)) {
        out << "Khong khoi tao duoc kich ban: " << error << '\n';
        return false;
    }
    while (!simulation.isFinished()) {
        if (!simulation.step()) {
            out << "Mo phong dung truoc khi ket thuc kich ban.\n";
            return false;
        }
    }
    printStatistics(simulation, out);
    captureComparisonRow(simulation, name, row);
    return true;
}

std::string comparisonTable(const ComparisonRow* rows, int count) {
    std::ostringstream report;
    report << "Bang so sanh 4 kich ban\n"
           << "Kich ban | Muc 0 TB/max | Muc 1 TB/max | Muc 2 TB/max"
           << " | Cho TB chung | Cho lon nhat | Phut ket thuc | Bat dau/hoan tat\n";
    for (int rowIndex = 0; rowIndex < count; ++rowIndex) {
        const ComparisonRow& row = rows[rowIndex];
        report << row.name;
        for (int level = 0; level < kPriorityCount; ++level) {
            const PriorityStats& values = row.byPriority[level];
            report << " | ";
            if (values.startedCount == 0) {
                report << "Chua co du lieu";
            } else {
                report << static_cast<double>(values.totalWait) / values.startedCount
                       << '/' << values.maxWait;
            }
        }
        report << " | ";
        if (row.hasAverageWait) report << row.averageWait;
        else report << "Chua co du lieu";
        report << " | " << row.maxSystemWaiting
               << " | " << row.finishMinute
               << " | " << row.started << '/' << row.completed << '\n';
    }
    return report.str();
}

void writeMinuteLog(const Simulation& simulation, const PatientStore& store,
                    std::ostream& log) {
    const int minute = simulation.lastProcessedMinute();
    printMinuteSnapshot(simulation, log);
    for (const PatientStore::Node* node = store.head(); node; node = node->next) {
        const Patient& patient = node->patient;
        if (patient.finishTime == minute)
            log << "Hoan tat: " << patient.input.id
                << " (phong " << patient.assignedRoomId << ")\n";
    }
    for (const PatientStore::Node* node = store.head(); node; node = node->next) {
        const Patient& patient = node->patient;
        if (patient.startTime == minute)
            log << "Goi kham: " << patient.input.id
                << " (phong " << patient.assignedRoomId
                << ", cho " << waitingTime(patient) << ")\n";
    }
}

void runToEndWithLog(Simulation& simulation, const PatientStore& store,
                     bool printEveryMinute, std::ostream& out,
                     std::ostream* log, const std::string& logPath,
                     bool& logFailed) {
    std::string error;
    if (!simulation.initialize(simulation.config(), error))
        throw std::runtime_error("Khong khoi tao duoc mo phong: " + error);
    while (!simulation.isFinished()) {
        if (!simulation.step())
            throw std::runtime_error("Mo phong dung truoc khi ket thuc.");
        if (printEveryMinute) printMinuteSnapshot(simulation, out);
        if (log != nullptr) {
            writeMinuteLog(simulation, store, *log);
            if (!*log) {
                out << "Ghi nhat ky that bai: " << logPath << '\n';
                logFailed = true;
                log = nullptr;
            }
        }
    }
    printStatistics(simulation, out);
}

} // namespace

void printSortedWaitReport(const PatientStore& store, std::ostream& out) {
    int count = 0;
    const Patient** report = makeSortedWaitReport(store, count);
    out << "Danh sach cho (sap xep giam dan):\n";
    if (count == 0 || report == nullptr) {
        out << "Chua co du lieu\n";
        delete[] report;
        return;
    }
    for (int index = 0; index < count; ++index) {
        const Patient* patient = report[index];
        out << (index + 1) << ". " << patient->input.id
            << " (" << patient->input.fullName << ")"
            << ", ticket=" << patient->input.ticketNumber
            << ", den=" << patient->input.arrivalTime
            << ", bat dau=" << patient->startTime
            << ", cho=" << waitingTime(*patient) << '\n';
    }
    delete[] report;
}

void printMinuteSnapshot(const Simulation& simulation, std::ostream& out) {
    out << "Phut " << simulation.lastProcessedMinute() << '\n';
    for (int index = 0; index < simulation.roomCount(); ++index) {
        const ClinicRoom& room = simulation.roomAt(index);
        out << "Phong " << room.id() << " (" << specialtyName(room.specialtyId())
            << "): cho thuong=" << room.normalQueueSize()
            << ", cho uu tien=" << room.priorityQueueSize();
        const Patient* current = room.currentPatient();
        if (current)
            out << ", dang kham=" << current->input.id
                << ", ket thuc=" << current->finishTime;
        else
            out << ", dang kham=khong";
        out << '\n';
    }
}

void printStatistics(const Simulation& simulation, std::ostream& out) {
    const Statistics& statistics = simulation.statistics();
    out << "Thong ke ("
        << (simulation.isFinished() ? "hoan tat" : "tam thoi") << ")\n";
    printPriorityStatistics(statistics, out);
    out << "Da hoan tat=" << statistics.completedCount()
        << ", cho he thong lon nhat=" << statistics.maxSystemWaiting() << '\n';
    for (int index = 0; index < simulation.roomCount(); ++index) {
        const RoomStats& room = statistics.byRoom(index);
        out << "Phong " << room.roomId << ": cho lon nhat=" << room.maxWaiting << '\n';
    }
}

void runToEnd(Simulation& simulation, bool printEveryMinute, std::ostream& out) {
    PatientStore emptyStore;
    bool logFailed = false;
    runToEndWithLog(simulation, emptyStore, printEveryMinute, out, nullptr,
                    std::string(), logFailed);
}

int runConsole(std::istream& in, std::ostream& out) {
    PatientStore store;
    Simulation simulation(store);
    bool initialized = false;

    const std::string workingPath = "data/patients_working.txt";
    std::error_code filesystemError;
    const bool hasWorkingFile = std::filesystem::exists(workingPath, filesystemError);
    if (filesystemError) {
        out << "Khong kiem tra duoc tep du lieu phien truoc: "
            << filesystemError.message() << '\n';
    } else if (!hasWorkingFile) {
        out << "Chua co du lieu phien truoc; kho benh nhan dang rong.\n";
    } else {
        const LoadResult result =
            loadPatients(workingPath, store, simulation.config().closingTime, out);
        if (result.opened && result.headerValid) {
            out << "Da nap du lieu phien truoc: chap nhan=" << result.accepted
                << ", tu choi=" << result.rejected << ".\n";
        }
    }

    for (;;) {
        out << "\n=== Clinic Queue Simulation ===\n"
            << "1. Nap tep benh nhan\n"
            << "2. Sinh bo du lieu\n"
            << "3. Chon cau hinh va khoi tao/chay lai\n"
            << "4. Tiep nhan mot benh nhan\n"
            << "5. Chay mot phut\n"
            << "6. Chay den het\n"
            << "7. Xem phong va thong ke\n"
            << "8. Luu du lieu dau vao / ket qua\n"
            << "9. So sanh 4 kich ban\n"
            << "0. Luu du lieu dau vao va thoat\n";
        int choice = -1;
        if (!readInt(in, out, "Lua chon: ", 0, 9, false, 0, choice)) return 0;

        if (choice == 0) {
            if (saveInput(in, out, store, "data/patients_working.txt", true)) return 0;
            if (!in) return 0;
            continue;
        }
        if (choice == 1) {
            if (initialized) {
                out << "Hay dung hoac dat lai phien mo phong truoc khi nap du lieu.\n";
                continue;
            }
            std::string path;
            if (!readPath(in, out, "Duong dan tep dau vao",
                          "data/patients_working.txt", path)) return 0;
            const LoadResult result = loadPatients(path, store, simulation.config().closingTime, out);
            if (result.opened && result.headerValid)
                out << "Da nap: chap nhan=" << result.accepted
                    << ", tu choi=" << result.rejected << ".\n";
            continue;
        }
        if (choice == 2) {
            if (initialized || !store.isEmpty()) {
                out << "Chi sinh du lieu vao kho rong va truoc khi khoi tao phien.\n";
                continue;
            }
            GeneratorConfig config;
            int number = 0;
            if (!readInt(in, out, "So benh nhan: ", 0,
                         std::numeric_limits<int>::max(), true,
                         config.patientCount, number)) return 0;
            config.patientCount = number;
            if (!readInt(in, out, "Phut dong tiep nhan: ", 1,
                         std::numeric_limits<int>::max() - 1, true,
                         config.closingTime, number)) return 0;
            config.closingTime = number;
            if (!readInt(in, out, "Thoi luong nho nhat: ", 1,
                         std::numeric_limits<int>::max(), true,
                         config.minServiceDuration, number)) return 0;
            config.minServiceDuration = number;
            if (!readInt(in, out, "Thoi luong lon nhat: ", config.minServiceDuration,
                         std::numeric_limits<int>::max(), true,
                         config.maxServiceDuration, number)) return 0;
            config.maxServiceDuration = number;
            if (!readUnsigned(in, out, "Seed unsigned int: ", config.seed)) return 0;
            std::string error;
            if (!generatePatients(config, store, error))
                out << "Sinh du lieu that bai: " << error << '\n';
            else
                out << "Da sinh " << store.size() << " benh nhan.\n";
            continue;
        }
        if (choice == 3) {
            SimulationConfig config;
            if (!promptConfiguration(in, out, simulation.config(), config)) {
                if (!in) return 0;
                continue;
            }
            std::string error;
            if (!simulation.initialize(config, error)) {
                out << "Khoi tao that bai: " << error << '\n';
                continue;
            }
            initialized = true;
            out << "Da khoi tao " << simulation.roomCount() << " phong.\n";
            continue;
        }
        if (choice == 4) {
            if (!initialized) {
                out << "Hay khoi tao mo phong truoc khi tiep nhan.\n";
                continue;
            }
            if (simulation.nextMinute() >= simulation.config().closingTime) {
                out << "Da dong tiep nhan.\n";
                continue;
            }
            for (;;) {
                std::string id;
                std::string name;
                if (!readLine(in, out, "Ma benh nhan: ", id) ||
                    !readLine(in, out, "Ho ten: ", name)) return 0;
                int age = 0;
                if (!readInt(in, out, "Tuoi [0, 120]: ", 0, 120, false, 0, age)) return 0;
                bool emergency = false;
                if (!readYesNo(in, out, "Cap cuu? (y/n): ", false, emergency)) return 0;
                int specialty = 0;
                if (!readInt(in, out, "Khoa [1, 3]: ", 1, 3, false, 0, specialty)) return 0;
                int duration = 0;
                if (!readInt(in, out, "Thoi luong kham (phut): ", 1,
                             std::numeric_limits<int>::max(),
                             false, 0, duration)) return 0;
                std::string error;
                if (simulation.admit(id, name, age, emergency, specialty, duration, error)) {
                    const Patient* admitted = store.findById(id);
                    out << "Da tiep nhan: ticket=" << admitted->input.ticketNumber
                        << ", muc uu tien=" << static_cast<int>(admitted->input.priority)
                        << ", phut den=" << admitted->input.arrivalTime
                        << ", phong=" << admitted->assignedRoomId << ".\n";
                    break;
                }

                out << "Tiep nhan that bai: " << error << '\n';
                bool retry = false;
                if (!readYesNo(in, out, "Nhap lai thong tin? (y/n): ", true, retry))
                    return 0;
                if (!retry) break;
            }
            continue;
        }
        if (choice == 5) {
            if (!initialized) {
                out << "Hay khoi tao mo phong truoc.\n";
            } else if (simulation.step()) {
                printMinuteSnapshot(simulation, out);
            } else {
                out << "Khong con phut nao can xu ly.\n";
            }
            continue;
        }
        if (choice == 6) {
            bool printEveryMinute = false;
            if (!readYesNo(in, out, "In tung phut? (y/n): ", false, printEveryMinute))
                return 0;
            bool writeLog = false;
            if (!readYesNo(in, out, "Ghi nhat ky ra tep? (y/n): ", false, writeLog))
                return 0;
            std::string logPath;
            std::ofstream logFile;
            std::ostream* log = nullptr;
            bool logFailed = false;
            if (writeLog) {
                if (!readPath(in, out, "Duong dan tep nhat ky",
                              "results/log.txt", logPath)) return 0;
                logFile.open(logPath.c_str(), std::ios::out | std::ios::binary |
                                                std::ios::trunc);
                if (!logFile.is_open())
                    out << "Khong mo duoc tep nhat ky de ghi: " << logPath << '\n';
                else
                    log = &logFile;
            }
            runToEndWithLog(simulation, store, printEveryMinute, out, log, logPath,
                            logFailed);
            if (logFile.is_open()) {
                logFile.close();
                if (!logFile && !logFailed)
                    out << "Hoan tat ghi nhat ky that bai: " << logPath << '\n';
                else if (!logFailed)
                    out << "Da ghi nhat ky vao " << logPath << ".\n";
            }
            initialized = true;
            continue;
        }
        if (choice == 7) {
            printMinuteSnapshot(simulation, out);
            printStatistics(simulation, out);
            printSortedWaitReport(store, out);
            continue;
        }
        if (choice == 8) {
            int saveChoice = 0;
            if (!readInt(in, out, "1: du lieu dau vao, 2: ket qua: ", 1, 2,
                         false, 0, saveChoice)) return 0;
            if (saveChoice == 1) {
                saveInput(in, out, store, "data/patients_working.txt", false);
            } else if (!initialized) {
                out << "Hay khoi tao mo phong truoc khi luu ket qua.\n";
            } else {
                std::string path;
                if (!readPath(in, out, "Duong dan tep ket qua",
                              "results/run_results.txt", path)) return 0;
                std::string error;
                if (!saveRunResults(path, store, simulation, error))
                    out << "Luu ket qua that bai: " << error << '\n';
                else
                    out << "Da luu ket qua vao " << path << ".\n";
            }
            continue;
        }

        const SimulationConfig previousConfig = simulation.config();
        SimulationConfig base = previousConfig;
        int closingTime = 0;
        if (!readInt(in, out, "Phut dong tiep nhan: ", 1,
                     std::numeric_limits<int>::max() - 1,
                     true, base.closingTime, closingTime)) return 0;
        base.closingTime = closingTime;

        ComparisonRow rows[4];
        bool comparisonSucceeded = true;
        out << "\n--- 1 phong, bat uu tien ---\n";
        SimulationConfig scenario = base;
        for (int& rooms : scenario.roomsPerSpecialty) rooms = 1;
        scenario.usePriority = true;
        comparisonSucceeded = runComparisonScenario(
            simulation, scenario, "1 phong, bat uu tien", rows[0], out);

        out << "\n--- 1 phong, tat uu tien ---\n";
        scenario.usePriority = false;
        comparisonSucceeded = comparisonSucceeded && runComparisonScenario(
            simulation, scenario, "1 phong, tat uu tien", rows[1], out);

        scenario = base;
        for (int& rooms : scenario.roomsPerSpecialty) rooms = 2;
        scenario.usePriority = true;
        out << "\n--- 2 phong, bat uu tien ---\n";
        comparisonSucceeded = comparisonSucceeded && runComparisonScenario(
            simulation, scenario, "2 phong, bat uu tien", rows[2], out);

        scenario.usePriority = false;
        out << "\n--- 2 phong, tat uu tien ---\n";
        comparisonSucceeded = comparisonSucceeded && runComparisonScenario(
            simulation, scenario, "2 phong, tat uu tien", rows[3], out);

        if (comparisonSucceeded) {
            std::string path;
            if (!readPath(in, out, "Duong dan bang so sanh",
                          "results/comparison.txt", path)) return 0;
            const std::string report = comparisonTable(rows, 4);
            out << report;
            std::string error;
            if (!saveComparisonResults(path, report, error))
                out << "Luu bang so sanh that bai: " << error << '\n';
            else
                out << "Da luu bang so sanh vao " << path << ".\n";
        }

        std::string restoreError;
        if (!simulation.initialize(previousConfig, restoreError)) {
            out << "Khong khoi tao lai duoc phien truoc: " << restoreError << '\n';
            initialized = false;
        } else {
            initialized = true;
            out << "Da khoi tao lai phien theo cau hinh truoc khi so sanh.\n";
        }
    }
}

} // namespace clinic
