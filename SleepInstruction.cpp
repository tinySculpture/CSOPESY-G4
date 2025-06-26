#include <sstream>

#include "Process.h"
#include "SleepInstruction.h"
#include "ConsoleUtil.h"

SleepInstruction::SleepInstruction(uint8_t ticks)
    : ticks(ticks) {
}

int SleepInstruction::execute(Process& process) {
    std::stringstream ss;
    ss << "SLEEP\t\tSleeping for " << static_cast<int>(ticks) << " ticks";
    process.addLog({ ConsoleUtil::generateTimestamp(), process.getCoreID(), ss.str() });
    return ticks;
}

std::string SleepInstruction::toString() const {
    return "SLEEP " + std::to_string(ticks);
}