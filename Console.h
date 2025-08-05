#pragma once

#include <map>
#include <memory>
#include <string>
#include <vector>

#include "Process.h"
#include "SystemConfig.h"
#include "Scheduler.h"
#include "MemoryManager.h"

class ConsoleLayout;

/**
 * @class ConsoleSystem
 * @brief Singleton class managing the console system, layouts, and configuration.
 *
 * The ConsoleSystem class provides a singleton interface for managing the console,
 * including drawing, input processing, layout switching, and configuration management.
 * It maintains the current and last layouts, and handles initialization and shutdown.
 */
class ConsoleSystem {
public:
    /**
     * @brief Retrieves the singleton instance of ConsoleSystem.
     * @return Pointer to the ConsoleSystem instance.
     */
    static ConsoleSystem* getInstance();

    /**
     * @brief Initializes the ConsoleSystem singleton.
     */
    static void initialize();

    /**
     * @brief Destroys the ConsoleSystem singleton and releases resources.
     */
    static void destroy();

    /**
     * @brief Draws the current console layout.
     */
    void drawConsole();

    /**
     * @brief Processes user input for the console.
     */
    void processInput();

    /**
     * @brief Switches to a different console layout by name.
     * @param layoutName Name of the layout to switch to.
     */
    void switchLayout(std::string layoutName);

    /**
     * @brief Switches to a different console layout and associates it with a process.
     * @param layoutName Name of the layout to switch to.
     * @param process Shared pointer to the process to associate with the layout.
     */
    void switchLayout(std::string layoutName, std::shared_ptr<Process> process);

    /**
     * @brief Configures the console system using a configuration file.
     * @param configFile Path to the configuration file.
     */
    void configure(const std::string& configFile);

    /**
     * @brief Retrieves the current system configuration.
     * @return Constant reference to the SystemConfig object.
     */
    const SystemConfig& getConfig() const;

    /**
     * @brief Checks if the console system is currently running.
     * @return True if running, false otherwise.
     */
    bool isRunning();

    /**
     * @brief Checks if the console system has been initialized.
     * @return True if initialized, false otherwise.
     */
    bool isInitialized();

    /**
     * @brief Exits the console system and performs cleanup.
     */
    void exit();

private:
    /**
     * @brief Private constructor for singleton pattern.
     */
    ConsoleSystem();

    /**
     * @brief Default destructor.
     */
    ~ConsoleSystem() = default;

    /**
     * @brief Copy constructor (deleted to enforce singleton).
     */
    ConsoleSystem(ConsoleSystem const&) {};

    /**
     * @brief Assignment operator (deleted to enforce singleton).
     */
    ConsoleSystem& operator=(ConsoleSystem const&) { return *this; }

    /**
     * @brief Pointer to the singleton instance.
     */
    static ConsoleSystem* sharedInstance;

    /**
     * @brief Map of available console layouts by name.
     */
    std::map<std::string, std::shared_ptr<ConsoleLayout>> layouts;

    /**
     * @brief Pointer to the current console layout.
     */
    std::shared_ptr<ConsoleLayout> currentLayout;

    /**
     * @brief Pointer to the last used console layout.
     */
    std::shared_ptr<ConsoleLayout> lastLayout;

    /**
     * @brief Indicates if the console system is running.
     */
    bool running = true;

    /**
     * @brief Indicates if the console system has been initialized.
     */
    bool initialized = false;

    /**
     * @brief System configuration object.
     */
    SystemConfig config;
};