﻿#pragma once

#include <memory>
#include <unordered_map>
#include <vector>

#include "Instruction.h"

/**
 * @class ForInstruction
 * @brief Executes a block of inner instructions multiple times, tracking per-process state.
 *
 * Supports nested loops via the 'layer' parameter. Each Process has its own LoopState,
 * so the same ForInstruction instance can be shared across processes without interference.
 */
class ForInstruction : public Instruction {
public:
    /**
     * @brief Construct a ForInstruction with the given iteration count and nesting layer.
     *
     * @param loopCount Number of iterations to perform.
     * @param layer     Current loop nesting level (0 for top-level loops).
     */
    ForInstruction(int loopCount, int layer);



    /**
     * @brief Add a sub-instruction to the loop body.
     *
     * @param instr Shared pointer to an Instruction that will be executed each iteration.
     */
    void addInstruction(const std::shared_ptr<Instruction>& instr);



    /**
     * @brief Execute one scheduling step of the loop for the given process.
     *
     * Manages initialization, logs the loop block once per iteration,
     * executes inner instructions (handling delays), and cleans up when done.
     *
     * @param process Reference to the executing Process.
     * @return int    Number of ticks to delay before the next execution (0 if none).
     */
    int execute(Process& process) override;



    /**
     * @brief Returns a concise text description of this loop.
     *
     * @return std::string e.g., "FOR X times".
     */
    std::string toString() const override;



    /**
     * @brief Check if the loop has completed for a given process.
     *
     * @param pid Process ID to query.
     * @return true if the loop state for the process no longer exists (completed).
     */
    bool isComplete(int pid) const override;

private:
    /**
     * @brief Internal state of the loop for each process.
     */
    struct LoopState {
        int counter = 0;            // How many iterations have been completed
        int index = 0;              // Index of the inner inner instruction to execute
        bool hasLogged = false;     // Whether the iteration header has been logged
        bool initialized = false;   // Whether the state has been initialized
        bool delaying = false;      // True if waiting out an instruction delay
    };

    int loopCount;  // Total number of iterations to perform
    int layer;      // Nesting Depth for log formatting of nested loops
    std::vector<std::shared_ptr<Instruction>> innerInstructions;    // Instruction to execute each iteration

    /**
     * @brief Per-process map from PID to its LoopState, enabling independent tracking.
     */
    mutable std::unordered_map<int, LoopState> processStates;



    /**
    * @brief Construct a nested log string representing the loop block and its contents.
    *
    * Recursively handles nested ForInstruction instances for proper formatting.
    *
    * @param process   Reference to the Process (for potential context).
    * @param loopDepth Current nesting depth (used for indentation or recursion).
    * @return std::string Full textual representation of the loop body.
    */
    std::string buildNestedLog(const Process& process, int loopDepth) const;
};