#pragma once

#include <cstdint>

#include "Instruction.h"

/**
 * @class SleepInstruction
 * @brief Concrete Instruction that puts the process to sleep for a given duration.
 *
 * When executed, this instruction logs a message indicating the sleep duration
 * and returns a tick count that the scheduler uses to delay process resumption.
 */
class SleepInstruction : public Instruction {
public:
    /**
     * @brief Construct a new SleepInstruction.
     *
     * @param ticks Number of scheduler ticks to sleep.
     */
    explicit SleepInstruction(uint8_t ticks);



    /**
     * @brief Execute the sleep operation for the given process.
     *
     * Logs a message with the sleep duration and returns the number of ticks
     * to delay the process before its next instruction.
     *
     * @param process Reference to the executing Process.
     * @return int Number of delay ticks (sleep duration).
     */
    int execute(Process& process) override;



    /**
     * @brief Returns a concise description of the instruction.
     *
     * @return std::string Always returns "SLEEP <ticks>".
     */
    std::string toString() const override;

private:
    uint8_t ticks;  // Number of ticks to sleep
};