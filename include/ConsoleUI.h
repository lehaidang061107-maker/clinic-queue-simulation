#ifndef CLINIC_CONSOLE_UI_H
#define CLINIC_CONSOLE_UI_H
#include <iosfwd>
#include "Simulation.h"
namespace clinic {
// Khong dung system("cls"), system("pause") hoac API chi Windows.
void printMinuteSnapshot(const Simulation&, std::ostream& out);
void printStatistics(const Simulation&, std::ostream& out);
// Tu initialize tung kich ban; chay den het, tuy chon in tung phut.
void runToEnd(Simulation&, bool printEveryMinute, std::ostream& out);
int runConsole(std::istream& in, std::ostream& out);
} // namespace clinic
#endif
