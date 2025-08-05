#pragma once

#include <atomic>
#include <mutex>
#include <thread>

#include "Process.h"
#include "Scheduler.h"
#include "Console.h"
#include "ConsoleLayout.h"
#include "Instruction.h"

/**
 * @class MainMenu
 * @brief Represents the main menu layout for the console application.
 * 
 * Inherits from ConsoleLayout and provides rendering, input processing,
 * and various screen and scheduler management functionalities.
 */
class MainMenu : public ConsoleLayout {
public:
    /**
     * @brief Constructs a MainMenu object.
     */
    MainMenu();

    /**
     * @brief Renders the main menu layout to the console.
     * @override
     */
    void render() override;

    /**
     * @brief Processes user input for the main menu.
     * @param input The input string entered by the user.
     * @return True if the input was processed successfully, false otherwise.
     * @override
     */
    bool processInput(const std::string input) override;

private:
    /**
     * @brief Indicates if the testing scheduler is active.
     */
    static std::atomic<bool> testingScheduler;

    /**
     * @brief Thread used for running tests in the background.
     */
    static std::thread testThread;

    /**
     * @brief Starts a new screen with the given name and memory size.
     * @param name The name of the screen.
     * @param memorySize The memory size to allocate.
     */
    void screenStart(const std::string& name, int memorySize);

    /**
     * @brief Resumes a previously started screen.
     * @param name The name of the screen to resume.
     */
    void screenResume(const std::string& name);

    /**
     * @brief Starts a custom screen with raw instructions.
     * @param name The name of the screen.
     * @param memorySize The memory size to allocate.
     * @param rawInstructionStr The raw instruction string.
     */
    void screenCustom(const std::string& name, uint16_t memorySize, const std::string& rawInstructionStr);

    /**
     * @brief Lists all available screens.
     */
    void screenList();

    /**
     * @brief Starts the scheduler.
     */
    void schedulerStart();

    /**
     * @brief Stops the scheduler.
     */
    void schedulerStop();

    /**
     * @brief Reports system utilization.
     */
    void reportUtil();

    /**
     * @brief Processes a system management interrupt (SMI).
     */
    void processSMI();

    /**
     * @brief Displays virtual memory statistics.
     */
    void VMStat();
};