#include <sstream>

#include "Process.h"
#include "SleepInstruction.h"
#include "ConsoleUtil.h"

SleepInstruction::SleepInstruction(uint8_t ticks)
    : ticks(ticks) {
}

int SleepInstruction::execute(Process& process) {
    // Build a log entry indicating the process is sleeping
    std::stringstream ss;
    ss << "SLEEP\t\tSleeping for " << static_cast<int>(ticks) << " ticks";
    process.addLog({ ConsoleUtil::generateTimestamp(), process.getCoreID(), ss.str() });

    // Return the sleep duration: scheduler will delay this process for "ticks"
    return ticks;
}

std::string SleepInstruction::toString() const {
    return "SLEEP " + std::to_string(ticks);
}