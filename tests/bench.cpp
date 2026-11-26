// Chuong trinh do hieu nang (giai doan 5). Chay bang `make bench`.
//
// Moi (N, lan chay r):
//   1. sinh N benh nhan (seed 2026, gio dong 480, thoi luong 5-15)   -> gen_ms
//   2. luu ra tep roi nap lai vao kho thu hai                        -> save_ms, load_ms
//   3. voi 4 cau hinh (1/2 phong x bat/tat uu tien) tren CUNG mot kho, chi initialize lai:
//        mo phong tu initialize den het ngay                         -> sim_ms
//        Merge Sort danh sach cho giam dan                           -> sort_ms
// gen/save/load do mot lan cho moi (N, r) va lap lai o 4 dong cau hinh cua lan do.
// Khong in tung phut, khong ghi nhat ky. Thoi gian la thoi gian may (ms), khong phai phut mo hinh;
// end_minute la phut ket thuc ngay trong mo hinh, de giai thich do doc cua sim_ms theo N.
#include <chrono>
#include <cstdio>
#include <cstdlib>
#include <ctime>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include "DataGenerator.h"
#include "FileIO.h"
#include "Simulation.h"
#include "Statistics.h"

using namespace clinic;
using Clock = std::chrono::steady_clock;

namespace {
const int kMaxSizes = 16;
const int kMaxReps = 64;

struct Options {
    int sizes[kMaxSizes] = {1000, 2000, 5000, 10000, 20000};
    int sizeCount = 5;
    int reps = 5;
    std::string machine = "local";
    std::string commit = "unknown";
    std::string flags = "unknown";
    std::string out = "results/bench_local.csv";
    std::string tmp = "build/bench_patients.tmp";
    bool outGiven = false;
};

struct Row {
    int n, rooms, priority, run;
    double gen, sim, sort, load, save;
    int endMinute;
};

double elapsedMs(Clock::time_point from, Clock::time_point to) {
    return std::chrono::duration<double, std::milli>(to - from).count();
}

// Sap tang bang chen (mang rat nho), tranh phu thuoc std::sort.
void sortAscending(double* values, int count) {
    for (int i = 1; i < count; ++i) {
        const double key = values[i];
        int j = i - 1;
        while (j >= 0 && values[j] > key) { values[j + 1] = values[j]; --j; }
        values[j + 1] = key;
    }
}
double median(const double* values, int count) {
    double copy[kMaxReps];
    for (int i = 0; i < count; ++i) copy[i] = values[i];
    sortAscending(copy, count);
    return count % 2 ? copy[count / 2] : (copy[count / 2 - 1] + copy[count / 2]) / 2.0;
}
double minimum(const double* values, int count) {
    double best = values[0];
    for (int i = 1; i < count; ++i) if (values[i] < best) best = values[i];
    return best;
}

bool parseSizes(const std::string& text, Options& options) {
    options.sizeCount = 0;
    std::size_t start = 0;
    while (start <= text.size()) {
        std::size_t comma = text.find(',', start);
        if (comma == std::string::npos) comma = text.size();
        const std::string piece = text.substr(start, comma - start);
        char* end = nullptr;
        const long value = std::strtol(piece.c_str(), &end, 10);
        if (piece.empty() || *end != '\0' || value < 1 || value > 1000000 ||
            options.sizeCount == kMaxSizes) return false;
        options.sizes[options.sizeCount++] = static_cast<int>(value);
        start = comma + 1;
    }
    return options.sizeCount > 0;
}

bool parseArgs(int argc, char** argv, Options& options) {
    for (int i = 1; i < argc; ++i) {
        const std::string arg = argv[i];
        if (i + 1 >= argc) { std::cerr << "Thieu gia tri cho " << arg << "\n"; return false; }
        const std::string value = argv[++i];
        if (arg == "--sizes") {
            if (!parseSizes(value, options)) { std::cerr << "--sizes sai: " << value << "\n"; return false; }
        } else if (arg == "--reps") {
            char* end = nullptr;
            const long reps = std::strtol(value.c_str(), &end, 10);
            if (*end != '\0' || reps < 1 || reps > kMaxReps) { std::cerr << "--reps phai 1.." << kMaxReps << "\n"; return false; }
            options.reps = static_cast<int>(reps);
        } else if (arg == "--machine") options.machine = value;
        else if (arg == "--commit") options.commit = value;
        else if (arg == "--flags") options.flags = value;
        else if (arg == "--out") { options.out = value; options.outGiven = true; }
        else if (arg == "--tmp") options.tmp = value;
        else { std::cerr << "Tuy chon khong biet: " << arg << "\n"; return false; }
    }
    if (!options.outGiven) options.out = "results/bench_" + options.machine + ".csv";
    return true;
}

const char* osName() {
#if defined(__APPLE__)
    return "macOS";
#elif defined(_WIN32)
    return "Windows";
#elif defined(__linux__)
    return "Linux";
#else
    return "unknown";
#endif
}

std::string compilerName() {
#if defined(__clang__)
    return std::string("clang ") + __clang_version__;
#elif defined(__GNUC__)
    return std::string("gcc ") + __VERSION__;
#elif defined(_MSC_VER)
    return "msvc " + std::to_string(_MSC_VER);
#else
    return "unknown";
#endif
}

std::string today() {
    char buffer[16];
    const std::time_t now = std::time(nullptr);
    std::strftime(buffer, sizeof buffer, "%Y-%m-%d", std::localtime(&now));
    return buffer;
}

bool fail(const std::string& message) {
    std::cerr << "LOI: " << message << "\n";
    return false;
}

// Mot lan chay cho (N, r): ghi 4 dong vao rows[rowCount..].
bool runOnce(const Options& options, int n, int run, Row* rows, int& rowCount) {
    std::string error;
    GeneratorConfig generator;
    generator.patientCount = n;
    PatientStore store;
    Clock::time_point t0 = Clock::now();
    if (!generatePatients(generator, store, error)) return fail("sinh du lieu: " + error);
    const double gen = elapsedMs(t0, Clock::now());
    if (store.size() != n) return fail("sinh sai so nguoi");

    t0 = Clock::now();
    if (!savePatients(options.tmp, store, error)) return fail("luu tep: " + error);
    const double save = elapsedMs(t0, Clock::now());
    double load = 0;
    {   // Kho nap lai chi de do va kiem tra; giai phong truoc khi mo phong de khong lam sai bo nho dinh.
        PatientStore loaded;
        std::ostringstream diagnostics;
        t0 = Clock::now();
        const LoadResult result = loadPatients(options.tmp, loaded, generator.closingTime, diagnostics);
        load = elapsedMs(t0, Clock::now());
        if (!result.opened || !result.headerValid || result.accepted != n || result.rejected != 0 ||
            loaded.size() != n)
            return fail("nap lai tep khong du " + std::to_string(n) + " ban ghi (" +
                        std::to_string(result.accepted) + " nhan, " + std::to_string(result.rejected) + " tu choi)");
    }
    for (int rooms = 1; rooms <= 2; ++rooms) {
        for (int priority = 1; priority >= 0; --priority) {
            SimulationConfig config;
            config.closingTime = generator.closingTime;
            config.usePriority = priority == 1;
            for (int& count : config.roomsPerSpecialty) count = rooms;

            Simulation simulation(store);
            t0 = Clock::now();
            if (!simulation.initialize(config, error)) return fail("initialize: " + error);
            while (simulation.step()) {}
            const double sim = elapsedMs(t0, Clock::now());
            if (!simulation.isFinished() || simulation.statistics().completedCount() != n)
                return fail("mo phong khong ket thuc du " + std::to_string(n) + " nguoi");

            int count = 0;
            t0 = Clock::now();
            const Patient** report = makeSortedWaitReport(store, count);
            const double sort = elapsedMs(t0, Clock::now());
            delete[] report;
            if (count != n) return fail("bao cao Merge Sort sai so nguoi");

            rows[rowCount++] = {n, rooms, priority, run, gen, sim, sort, load, save,
                                simulation.lastProcessedMinute()};
        }
    }
    return true;
}

void writeHeader(std::ostream& out, const Options& options) {
    out << "# Clinic Queue Simulator - bench giai doan 5\n"
        << "# machine: " << options.machine << "\n"
        << "# os: " << osName() << "\n"
        << "# compiler: " << compilerName() << "\n"
        << "# flags: " << options.flags << "\n"
#ifdef __OPTIMIZE__
        << "# optimized: yes\n"
#else
        << "# optimized: no\n"
#endif
        << "# commit: " << options.commit << "\n"
        << "# date: " << today() << "\n"
        << "# data: generatePatients seed=2026 closing=480 duration=5-15\n"
        << "# reps: " << options.reps << " (gen/save/load do mot lan moi (n,run), lap o 4 dong cau hinh)\n"
        << "# units: *_ms la thoi gian may (ms); end_minute la phut ket thuc ngay trong mo hinh\n"
        << "n,rooms,priority,run,gen_ms,sim_ms,sort_ms,load_ms,save_ms,end_minute\n";
}
}  // namespace

int main(int argc, char** argv) {
    Options options;
    if (!parseArgs(argc, argv, options)) return 2;
#ifndef __OPTIMIZE__
    std::cerr << "CANH BAO: khong bien dich voi -O1/-O2; so do khong dung cho bao cao.\n";
#endif
    std::ofstream out(options.out.c_str());
    if (!out) {
        fail("khong mo duoc " + options.out + " (thu muc results/ co ton tai khong?)");
        return 1;
    }
    writeHeader(out, options);
    out << std::fixed;
    out.precision(3);

    std::cout << "Bench: " << options.machine << ", " << options.reps << " lan/diem, ghi " << options.out << "\n";
    std::printf("%7s %5s %4s | %10s %10s | %9s %9s %9s %9s | %9s\n", "N", "rooms", "prio", "sim_med", "sim_min",
                "gen_med", "sort_med", "load_med", "save_med", "end_min");
    for (int s = 0; s < options.sizeCount; ++s) {
        const int n = options.sizes[s];
        Row* rows = new Row[4 * options.reps];
        int rowCount = 0;
        for (int run = 1; run <= options.reps; ++run) {
            if (!runOnce(options, n, run, rows, rowCount)) { delete[] rows; return 1; }
        }
        for (int i = 0; i < rowCount; ++i)
            out << rows[i].n << ',' << rows[i].rooms << ',' << rows[i].priority << ',' << rows[i].run << ','
                << rows[i].gen << ',' << rows[i].sim << ',' << rows[i].sort << ',' << rows[i].load << ','
                << rows[i].save << ',' << rows[i].endMinute << '\n';
        out.flush();
        for (int rooms = 1; rooms <= 2; ++rooms) {
            for (int priority = 1; priority >= 0; --priority) {
                double sim[kMaxReps], gen[kMaxReps], sort[kMaxReps], load[kMaxReps], save[kMaxReps];
                int count = 0, endMinute = 0;
                for (int i = 0; i < rowCount; ++i) {
                    if (rows[i].rooms != rooms || rows[i].priority != priority) continue;
                    sim[count] = rows[i].sim; gen[count] = rows[i].gen; sort[count] = rows[i].sort;
                    load[count] = rows[i].load; save[count] = rows[i].save;
                    endMinute = rows[i].endMinute;
                    ++count;
                }
                std::printf("%7d %5d %4d | %10.2f %10.2f | %9.2f %9.2f %9.2f %9.2f | %9d\n", n, rooms, priority,
                            median(sim, count), minimum(sim, count), median(gen, count), median(sort, count),
                            median(load, count), median(save, count), endMinute);
            }
        }
        delete[] rows;
    }
    std::remove(options.tmp.c_str());
    if (!out) {
        fail("ghi " + options.out + " that bai");
        return 1;
    }
    std::cout << "Xong. Thoi gian la ms may; xem CSV de co tung lan do.\n";
    return 0;
}
