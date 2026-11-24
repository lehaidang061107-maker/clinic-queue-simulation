#include "FileIO.h"

#include "Statistics.h"

#include <charconv>
#include <fstream>
#include <memory>
#include <new>
#include <string>
#include <system_error>

namespace clinic {
namespace {

const char* const kHeader =
    "id|fullName|age|priority|specialtyId|ticketNumber|arrivalTime|serviceDuration";

bool parseInteger(const std::string& text, int& value) {
    if (text.empty()) return false;
    const char* first = text.data();
    const char* last = first + text.size();
    const std::from_chars_result result = std::from_chars(first, last, value);
    return result.ec == std::errc() && result.ptr == last;
}

bool splitRecord(const std::string& line, std::string fields[8]) {
    std::size_t start = 0;
    for (int index = 0; index < 8; ++index) {
        const std::size_t delimiter = line.find('|', start);
        if (index < 7 && delimiter == std::string::npos) return false;
        if (index == 7 && delimiter != std::string::npos) return false;
        const std::size_t end = delimiter == std::string::npos ? line.size() : delimiter;
        fields[index] = line.substr(start, end - start);
        start = end + 1;
    }
    return true;
}

void reportLineError(std::ostream& diagnostics, std::size_t lineNumber,
                     const std::string& reason) {
    diagnostics << "Dong " << lineNumber << ": " << reason << '\n';
}

const char* priorityName(Priority priority) {
    switch (priority) {
    case Priority::Emergency: return "Cap cuu";
    case Priority::Preferential: return "Uu tien";
    case Priority::Normal: return "Thong thuong";
    }
    return "Khong hop le";
}

} // namespace

LoadResult loadPatients(const std::string& path, PatientStore& store,
                        int closingTime, std::ostream& diagnostics) {
    LoadResult result;
    if (!store.isEmpty()) {
        diagnostics << "Khong the nap tep vao kho khong rong.\n";
        return result;
    }

    std::ifstream input(path.c_str(), std::ios::in | std::ios::binary);
    if (!input.is_open()) {
        diagnostics << "Khong mo duoc tep: " << path << '\n';
        return result;
    }
    result.opened = true;

    std::string line;
    if (!std::getline(input, line)) {
        diagnostics << "Tep rong hoac khong doc duoc tieu de: " << path << '\n';
        return result;
    }
    if (!line.empty() && line[line.size() - 1] == '\r') line.pop_back();
    if (line != kHeader) {
        diagnostics << "Tieu de khong dung dinh dang 8 truong.\n";
        return result;
    }
    result.headerValid = true;

    std::size_t lineNumber = 1;
    try {
        while (std::getline(input, line)) {
            ++lineNumber;
            if (!line.empty() && line[line.size() - 1] == '\r') line.pop_back();
            if (line.empty()) continue;

            std::string fields[8];
            if (!splitRecord(line, fields)) {
                ++result.rejected;
                reportLineError(diagnostics, lineNumber, "Can dung 8 truong phan cach bang '|'.");
                continue;
            }

            int age = 0;
            int priority = 0;
            int specialty = 0;
            int ticket = 0;
            int arrival = 0;
            int duration = 0;
            if (!parseInteger(fields[2], age) || !parseInteger(fields[3], priority) ||
                !parseInteger(fields[4], specialty) || !parseInteger(fields[5], ticket) ||
                !parseInteger(fields[6], arrival) || !parseInteger(fields[7], duration)) {
                ++result.rejected;
                reportLineError(diagnostics, lineNumber, "Truong so phai la so nguyen hop le.");
                continue;
            }
            if (priority < static_cast<int>(Priority::Emergency) ||
                priority > static_cast<int>(Priority::Normal)) {
                ++result.rejected;
                reportLineError(diagnostics, lineNumber, "Muc uu tien phai la 0, 1 hoac 2.");
                continue;
            }

            PatientInput patient{fields[0], fields[1], age,
                                 static_cast<Priority>(priority), specialty, ticket,
                                 arrival, duration};
            std::string error;
            if (!validatePatientInput(patient, closingTime, error)) {
                ++result.rejected;
                reportLineError(diagnostics, lineNumber, error);
                continue;
            }
            if (!store.add(patient, error)) {
                if (error == "Khong du bo nho de them benh nhan.") {
                    diagnostics << "Loi cap phat bo nho tai dong " << lineNumber
                                << "; huy du lieu da nap.\n";
                    store.clear();
                    result.accepted = 0;
                    result.opened = false;
                    return result;
                }
                ++result.rejected;
                reportLineError(diagnostics, lineNumber, error);
                continue;
            }
            ++result.accepted;
        }
    } catch (const std::bad_alloc&) {
        diagnostics << "Khong du bo nho khi doc dong " << lineNumber
                    << "; huy du lieu da nap.\n";
        store.clear();
        result.accepted = 0;
        result.opened = false;
        return result;
    }

    if (input.bad()) {
        diagnostics << "Loi doc tep sau dong " << lineNumber
                    << "; huy du lieu da nap: " << path << '\n';
        store.clear();
        result.accepted = 0;
        result.opened = false;
    }
    return result;
}

bool savePatients(const std::string& path, const PatientStore& store,
                  std::string& error) {
    error.clear();
    std::ofstream output(path.c_str(), std::ios::out | std::ios::binary | std::ios::trunc);
    if (!output.is_open()) {
        error = "Khong mo duoc tep de ghi: " + path;
        return false;
    }

    output << kHeader << '\n';
    for (const PatientStore::Node* node = store.head(); node; node = node->next) {
        const PatientInput& patient = node->patient.input;
        output << patient.id << '|' << patient.fullName << '|' << patient.age << '|'
               << static_cast<int>(patient.priority) << '|' << patient.specialtyId << '|'
               << patient.ticketNumber << '|' << patient.arrivalTime << '|'
               << patient.serviceDuration << '\n';
        if (!output) {
            error = "Loi ghi du lieu vao tep: " + path;
            return false;
        }
    }
    output.close();
    if (!output) {
        error = "Loi hoan tat ghi tep: " + path;
        return false;
    }
    return true;
}

bool saveRunResults(const std::string& path, const PatientStore& store,
                    const Simulation& simulation, std::string& error) {
    error.clear();
    std::ofstream output(path.c_str(), std::ios::out | std::ios::binary | std::ios::trunc);
    if (!output.is_open()) {
        error = "Khong mo duoc tep de ghi: " + path;
        return false;
    }

    const SimulationConfig& config = simulation.config();
    const Statistics& statistics = simulation.statistics();
    output << "Clinic Queue Simulation - "
           << (simulation.isFinished() ? "Ket qua hoan tat" : "Ket qua tam thoi") << '\n'
           << "Closing time: " << config.closingTime << '\n'
           << "Rooms per specialty: " << config.roomsPerSpecialty[0] << '|'
           << config.roomsPerSpecialty[1] << '|' << config.roomsPerSpecialty[2] << '\n'
           << "Priority enabled: " << (config.usePriority ? "yes" : "no") << '\n'
           << "Last processed minute: " << simulation.lastProcessedMinute() << '\n'
           << "Started: ";
    int started = 0;
    for (int level = 0; level < kPriorityCount; ++level)
        started += statistics.byPriority(static_cast<Priority>(level)).startedCount;
    output << started << '\n'
           << "Completed: " << statistics.completedCount() << '\n'
           << "Maximum system waiting: " << statistics.maxSystemWaiting() << '\n'
           << "\nStatistics by priority\n";
    for (int level = 0; level < kPriorityCount; ++level) {
        const Priority priority = static_cast<Priority>(level);
        const PriorityStats& values = statistics.byPriority(priority);
        output << level << '|' << priorityName(priority) << '|' << values.startedCount << '|'
               << values.totalWait << '|' << values.maxWait << '|';
        if (values.startedCount == 0)
            output << "no data\n";
        else
            output << static_cast<double>(values.totalWait) / values.startedCount << '\n';
    }

    output << "\nRoom maximum waiting\n";
    for (int index = 0; index < simulation.roomCount(); ++index) {
        const RoomStats& room = statistics.byRoom(index);
        output << room.roomId << '|' << room.maxWaiting << '\n';
    }

    int reportCount = 0;
    std::unique_ptr<const Patient*[]> report(makeSortedWaitReport(store, reportCount));
    output << "\nWait report (descending wait, ascending ticket)\n"
           << "id|ticket|arrivalTime|startTime|wait\n";
    for (int index = 0; index < reportCount; ++index) {
        const Patient& patient = *report[index];
        output << patient.input.id << '|' << patient.input.ticketNumber << '|'
               << patient.input.arrivalTime << '|' << patient.startTime << '|'
               << waitingTime(patient) << '\n';
        if (!output) {
            error = "Loi ghi bao cao vao tep: " + path;
            return false;
        }
    }

    output.close();
    if (!output) {
        error = "Loi hoan tat ghi tep: " + path;
        return false;
    }
    return true;
}

bool saveComparisonResults(const std::string& path, const std::string& report,
                           std::string& error) {
    error.clear();
    std::ofstream output(path.c_str(), std::ios::out | std::ios::binary | std::ios::trunc);
    if (!output.is_open()) {
        error = "Khong mo duoc tep de ghi: " + path;
        return false;
    }

    output << report;
    if (!output) {
        error = "Loi ghi bang so sanh vao tep: " + path;
        return false;
    }
    output.close();
    if (!output) {
        error = "Loi hoan tat ghi bang so sanh vao tep: " + path;
        return false;
    }
    return true;
}

} // namespace clinic
