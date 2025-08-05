#include <algorithm>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <random>


#include "MainMenu.h"
#include "ProcessScreen.h"
#include "ColorUtil.h"
#include "ConsoleUtil.h"
#include "GlobalScheduler.h"
#include "InstructionGenerator.h"
#include "MemoryManager.h"
#include "Globals.h"

using Color = ColorUtil::Color;
namespace CU = ColorUtil;

std::thread MainMenu::testThread;
std::atomic<bool> MainMenu::testingScheduler{ false };

MainMenu::MainMenu()
    : ConsoleLayout("MainMenu") {
}

void MainMenu::render() {

    CU::printColoredText(CU::Color::Purple, "╔═════════════════════════════════════════════════════════════════════════════════════════════════════════╗\n");
    CU::printColoredText(CU::Color::Purple, "║  ██████╗███████╗ ██████╗ ██████╗ ███████╗███████╗██╗   ██╗      ███████╗███╗   ███╗██╗   ██╗            ║\n");
    CU::printColoredText(CU::Color::Purple, "║ ██╔════╝██╔════╝██╔═══██╗██╔══██╗██╔════╝██╔════╝╚██╗ ██╔╝      ██╔════╝████╗ ████║██║   ██║            ║\n");
    CU::printColoredText(CU::Color::Purple, "║ ██║     ███████╗██║   ██║██████╔╝█████╗  ███████╗ ╚████╔╝       █████╗  ██╔████╔██║██║   ██║            ║\n");
    CU::printColoredText(CU::Color::Purple, "║ ██║     ╚════██║██║   ██║██╔═══╝ ██╔══╝  ╚════██║  ╚██╔╝        ██╔══╝  ██║╚██╔╝██║██║   ██║            ║\n");
    CU::printColoredText(CU::Color::Purple, "║ ╚██████╗███████║╚██████╔╝██║     ███████╗███████║   ██║         ███████╗██║ ╚═╝ ██║ ██████╔╝            ║\n");
    CU::printColoredText(CU::Color::Purple, "║  ╚═════╝╚══════╝ ╚═════╝ ╚═╝     ╚══════╝╚══════╝   ╚═╝         ╚══════╝╚═╝     ╚═╝ ╚═════╝             ║\n");
    CU::printColoredText(CU::Color::Purple, "║                                                                                          Console v2.0   ║\n");
    CU::printColoredText(CU::Color::Purple, "╠═════════════════════════════════════════════════════════════════════════════════════════════════════════╣\n");
    CU::printColoredText(CU::Color::Purple, "║ █▀▀ █▀█ █▄░█ █▀ █▀█ █░░ █▀▀   █ █▄░█ ▀█▀ █▀▀ █▀█ █▀▀ ▄▀█ █▀▀ █▀▀                                        ║\n");
    CU::printColoredText(CU::Color::Purple, "║ █▄▄ █▄█ █░▀█ ▄█ █▄█ █▄▄ ██▄   █ █░▀█ ░█░ ██▄ █▀▄ █▀░ █▀█ █▄▄ ██▄                                        ║\n");
    CU::printColoredText(CU::Color::Purple, "╚═════════════════════════════════════════════════════════════════════════════════════════════════════════╝\n");

    std::cout << std::endl;
    CU::printColoredText(CU::Color::Aqua, "┌─ Development Team ───────────────────────────────────────────────────────────────────────────────────────┐\n");
    CU::printColoredText(CU::Color::Aqua, "│                                                                                                          │\n");
    CU::printColoredText(CU::Color::Aqua, "│  > Luistro, Josiah Mari                                                                                  │\n");
    CU::printColoredText(CU::Color::Aqua, "│  > Rimando, Alonzo Andrei                                                                                │\n");
    CU::printColoredText(CU::Color::Aqua, "│  > Santos, Andrea Li                                                                                     │\n");
    CU::printColoredText(CU::Color::Aqua, "│  > Vasquez, Ryan Clemence                                                                                │\n");
    CU::printColoredText(CU::Color::Aqua, "│                                                                                                          │\n");
    CU::printColoredText(CU::Color::Aqua, "│ Last Updated: ");
    CU::printColoredText(CU::Color::Aqua, "08-05-2025                                                                                 │\n");
    CU::printColoredText(CU::Color::Aqua, "└──────────────────────────────────────────────────────────────────────────────────────────────────────────┘\n");


    std::cout << std::endl;
    std::cout << "┌─ Available Commands ─────────────────────────────────────────────────────────────────────────────────────┐" << std::endl;
    std::cout << "│                                                                                                          │" << std::endl;
    std::cout << "│  [01] initialize                                            - Initialize the system environment          │" << std::endl;
    std::cout << "│  [02] screen -s <name> <memory size>                        - Start a new screen session                 │" << std::endl;
    std::cout << "│  [03] screen -r <name>                                      - Resume an existing screen session          |" << std::endl;
    std::cout << "│  [04] screen -c <name> <memory size> \"<instructions>\"       - Create a custom screen session             │" << std::endl;
    std::cout << "│  [05] screen -ls                                            - List all existing screen sessions          │" << std::endl;
    std::cout << "│  [06] scheduler-start                                       - Run scheduler suite                        │" << std::endl;
    std::cout << "│  [07] scheduler-stop                                        - Stop all scheduler processes               │" << std::endl;
    std::cout << "│  [08] report-util                                           - Generate system utilization report         │" << std::endl;
    std::cout << "│  [09] process-smi                                           - Process System Management Interrupt (SMI)  │" << std::endl;
    std::cout << "│  [10] vmstat                                                - Display virtual memory statistics          │" << std::endl;
    std::cout << "│  [11] clear                                                 - Clear the console display                  │" << std::endl;
    std::cout << "│  [12] exit                                                  - Exit the console emulator                  │" << std::endl;
    std::cout << "│                                                                                                          │" << std::endl;
    std::cout << "└──────────────────────────────────────────────────────────────────────────────────────────────────────────┘" << std::endl;
}

bool MainMenu::processInput(const std::string input) {
    // Tokenize the input string into command and arguments
    std::vector<std::string> tokens = ConsoleUtil::tokenizeInput(input);

    if (tokens.empty()) {
        CU::printColoredText(Color::Red, "[X] Error: Empty input.\n");
        return false;
    }

    const std::string& command = tokens[0];

    // Handle "exit" command
    if (command == "exit") {
        if (ConsoleSystem::getInstance()->isInitialized()) {
            CU::printColoredText(Color::Yellow, "[!] Exiting the system. Please wait...\n");
            GlobalScheduler::getInstance()->stop();
            ConsoleSystem::getInstance()->exit();
        }
        ConsoleSystem::getInstance()->exit();
    }
    // Handle "clear" command
    else if (command == "clear") {
        ConsoleSystem::getInstance()->switchLayout("MainMenu");
        ConsoleSystem::getInstance()->drawConsole();
    }
    // Handle "initialize" command
    else if (command == "initialize") {
        if (ConsoleSystem::getInstance()->isInitialized()) {
            CU::printColoredText(Color::Red, "[X] System is already initialized.\n");
            return false;
        }
        ConsoleSystem::getInstance()->configure("config.txt");
        GlobalScheduler::getInstance()->start();
    }
    // Handle commands that require system initialization
    else if (command == "scheduler-start" || command == "scheduler-stop" || command == "report-util" || command == "screen" || command == "process-smi" || command == "vmstat") {
        if (!ConsoleSystem::getInstance()->isInitialized()) {
            CU::printColoredText(Color::Red, "[X] Please initialize the system first.\n");
            return false;
        }

        // Handle "screen" subcommands
        if (command == "screen") {
            // Start a new screen session
            if (tokens.size() == 4 && tokens[1] == "-s") {
                const std::string& process_name = tokens[2];
                try {
                    int memSize = std::stoi(tokens[3]);
                    screenStart(process_name, memSize);
                } catch (...) {
                    CU::printColoredText(Color::Red, "[X] Invalid memory size. Usage: screen -s <name> <memory_size>\n");
                }
            }
            // Resume an existing screen session
            else if (tokens.size() == 3 && tokens[1] == "-r") {
                screenResume(tokens[2]);
            }
            // List all existing screen sessions
            else if (tokens.size() == 2 && tokens[1] == "-ls") {
                screenList();
            }
            // Create a custom screen session
            else if (tokens.size() >= 5 && tokens[1] == "-c") {
                const std::string& process_name = tokens[2];
                std::string memSizeStr = tokens[3];
                std::string rawInstructions;

                try {
                    uint16_t memSize = static_cast<uint16_t>(std::stoul(memSizeStr));

                    // Concatenate all instruction tokens into a single string
                    for (size_t i = 4; i < tokens.size(); ++i) {
                        if (i > 4) rawInstructions += " ";
                        rawInstructions += tokens[i];
                    }

                    rawInstructions = ConsoleUtil::trim(rawInstructions);
                    // Remove leading and trailing quotes if present
                    if (!rawInstructions.empty() && rawInstructions.front() == '"')
                        rawInstructions.erase(0, 1);
                    if (!rawInstructions.empty() && rawInstructions.back() == '"')
                        rawInstructions.pop_back();

                    screenCustom(process_name, memSize, rawInstructions);
                } catch (...) {
                    CU::printColoredText(Color::Red, "[X] Invalid memory size or instruction format.\n");
                }
            }
            // Invalid usage of "screen" command
            else {
                CU::printColoredText(Color::Red, "[X] Proper Usage: screen -s <name> <memory_size>, screen -r <name>, screen -c <name> <memory_size> \"<instructions>\", or screen -ls.\n");
            }
        }
        // Start scheduler test batch
        else if (command == "scheduler-start") {
            schedulerStart();
        }
        // Stop scheduler test batch
        else if (command == "scheduler-stop") {
            schedulerStop();
        }
        // Generate system utilization report
        else if (command == "report-util") {
            reportUtil();
        }
        // Process System Management Interrupt (SMI)
        else if (command == "process-smi") {
            processSMI();
        }
        // Display virtual memory statistics
        else if (command == "vmstat") {
            VMStat();
        }
    }
    // Handle invalid commands
    else {
        CU::printColoredText(Color::Red, "[X] Invalid Command. Please Try Again.\n");
    }

    return false;
}

void MainMenu::screenStart(const std::string& process_name, int memorySize) {
    const SystemConfig& config = ConsoleSystem::getInstance()->getConfig();

    // Check if memory size is within allowed range
    if (memorySize < config.minMemoryPerProcess || memorySize > config.maxMemoryPerProcess) {
        CU::printColoredText(Color::Red, "[X] Memory size must be between " + std::to_string(config.minMemoryPerProcess) +
                              " and " + std::to_string(config.maxMemoryPerProcess) + " bytes.\n");
        return;
    }

    // Check if memory size is a power of 2
    if ((memorySize & (memorySize - 1)) != 0) {
        CU::printColoredText(Color::Red, "[X] Memory size must be a power of 2.\n");
        return;
    }

    // Check if a process with the same name already exists
    auto process = ConsoleUtil::findProcessByName(process_name);
    if (process) {
        CU::printColoredText(Color::Red, "[X] Process \"" + process_name + "\" already exists.\n");
        return;
    }

    // Calculate number of pages needed for the process
    uint32_t pagesNeeded = (memorySize + config.memoryPerFrame - 1) / config.memoryPerFrame;

    // Generate instructions for the new process
    auto instructions = InstructionGenerator::generateInstructions(
                        Process::peakNextPID(), config, memorySize
                    );

    // Create the new process object
    auto newProcess = std::make_shared<Process>(name, instructions, memorySize, pagesNeeded);

    // Allocate page table for the process
    MemoryManager::getInstance()->allocatePageTable(newProcess);
    // Add process to the scheduler
    GlobalScheduler::getInstance()->addProcess(newProcess);
    // Register the process globally
    Process::registerProcess(newProcess);

    // Notify user of successful process creation
    CU::printColoredText(Color::Green, "[*] Process \"" + process_name + "\" created with " + 
        std::to_string(memorySize) + " bytes of memory (" + 
        std::to_string(pagesNeeded) + " pages).\n");

    // Warn if memory size is too small for variable storage
    if (memorySize < 64) {
        CU::printColoredText(Color::Yellow, "[!] Note: Process will not be able to store variables as its memory size is less than 64 bytes.\n");
    }
}


void MainMenu::screenResume(const std::string& process_name) {
    // Find the process by name
    auto process = ConsoleUtil::findProcessByName(process_name);

    // If process does not exist, notify user
    if (!process) {
        CU::printColoredText(Color::Red, "[X] No such process found.\n");
        return;
    }

    // If process is already finished, notify user
    if (process->isFinished()) {
        CU::printColoredText(Color::Red, "[X] Process \"" + process_name + "\" is already finished.\n");
        return;
    }

    // If process is terminated, notify user with reason
    if (process->isTerminated()) {
        if (process->isTerminatedByMemoryViolation()) {
            // Terminated due to memory violation, show address and timestamp
            CU::printColoredText(Color::Red,
                "[X] Process \"" + process_name + "\" shut down due to memory access violation error that occurred at " +
                process->getTerminationTimestamp() + ". 0x" +
                ConsoleUtil::toHex(process->getInvalidMemoryAddress()) + " is invalid.\n");
        } else {
            // Terminated for other reasons
            CU::printColoredText(Color::Red, "[X] Process \"" + process_name + "\" has been terminated.\n");
        }
        return;
    }

    // Switch to the process screen for the selected process
    ConsoleSystem::getInstance()->switchLayout("ProcessScreen", process);
}

void MainMenu::screenCustom(const std::string& name, uint16_t memorySize, const std::string& rawInstructionStr) {
    // Get system configuration
    const SystemConfig& config = ConsoleSystem::getInstance()->getConfig();

    // Print process creation info
    CU::printColoredText(Color::Blue, "[*] Creating custom process with the following parameters:\n");
    CU::printColoredText(Color::Blue, "Process Name: " + name + "\n");
    CU::printColoredText(Color::Blue, "Memory Size: " + std::to_string(memorySize) + " bytes\n");
    CU::printColoredText(Color::Blue, "Instructions: " + rawInstructionStr + "\n");

    // Split the raw instruction string by ';' and trim each instruction
    std::vector<std::string> instructionStrings;
    std::stringstream ss(rawInstructionStr);
    std::string token;
    while (std::getline(ss, token, ';')) {
        std::string trimmed = ConsoleUtil::trim(token);
        if (!trimmed.empty()) instructionStrings.push_back(trimmed);
    }

    // Validate instruction count
    if (instructionStrings.empty() || instructionStrings.size() > 50) {
        CU::printColoredText(Color::Red, "[X] Invalid command. Instruction count must be between 1 and 50.\n");
        return;
    }

    // Parse each instruction string into an Instruction object
    std::vector<std::shared_ptr<Instruction>> instructions;
    try {
        for (const auto& rawStr : instructionStrings) {
            std::string cleanStr = ConsoleUtil::unescapeString(rawStr); // Unescape special characters
            instructions.push_back(Instruction::fromString(cleanStr));  // Parse to Instruction
        }
    } catch (const std::exception& e) {
        CU::printColoredText(Color::Red, std::string("[X] Error parsing instruction: ") + e.what() + "\n");
        return;
    }

    // Calculate number of pages needed for the process
    uint32_t pagesNeeded = (memorySize + config.memoryPerFrame - 1) / config.memoryPerFrame;

    // Create the process and register it
    auto process = std::make_shared<Process>(name, instructions, memorySize, pagesNeeded);
    Process::registerProcess(process);
    MemoryManager::getInstance()->allocatePageTable(process);
    GlobalScheduler::getInstance()->addProcess(process);

    // Notify user of successful creation
    CU::printColoredText(Color::Green, "[*] Process \"" + name + "\" created with " +
        std::to_string(instructionStrings.size()) + " custom instruction(s).\n");
}


void MainMenu::screenList() {
    // Get scheduler, all processes, and CPU cores
    auto scheduler = GlobalScheduler::getInstance();
    auto processes = scheduler->getAllProcesses();
    auto cores = scheduler->getCores();

    // Get system configuration for CPU info
    const SystemConfig& config = ConsoleSystem::getInstance()->getConfig();
    int totalCores = config.numCPU;

    // Track which cores are currently used
    std::unordered_set<int> usedCores;
    for (const auto& core : cores) {
        if (!(core->isFree()))
            usedCores.insert(core->getId());
    }

    int coresUsed = static_cast<int>(usedCores.size());
    double cpuUtil = (static_cast<double>(coresUsed) / totalCores) * 100.0;

    // Print CPU utilization summary
    std::cout << "=====================================================================\n";
    std::cout << "CPU Utilization: " << std::fixed << std::setprecision(1) << cpuUtil << "%\n";
    std::cout << "Cores Used:      " << coresUsed << "\n";
    std::cout << "Cores Available: " << (totalCores - coresUsed) << "\n";
    std::cout << "=====================================================================\n\n";

    // Print running processes header
    CU::printColoredText(Color::Blue, "Running Processes\n");
    std::cout << "---------------------------------------------------------------------\n";
    std::cout << std::left
        << std::setw(6) << "PID"
        << std::setw(12) << "Name"
        << std::setw(28) << "Created"
        << std::setw(6) << "Core"
        << std::setw(12) << "Status"
        << std::setw(10) << "Instr.";
    std::cout << "\n--------------------------------------------------------------------\n";

    bool anyRunning = false;
    // List all running processes (not finished or terminated)
    for (const auto& p : processes) {
        if (!(p->isFinished() || p->isTerminated())) {
            anyRunning = true;
            std::string name = ConsoleUtil::truncateLongNames(p->getName());
            std::string stateStr;
            // Get process state as string
            switch (p->getState()) {
            case ProcessState::Blocked: stateStr = "Blocked"; break;
            case ProcessState::Ready: stateStr = "Ready"; break;
            case ProcessState::Running: stateStr = "Running"; break;
            case ProcessState::Sleeping: stateStr = "Sleeping"; break;
            }
            // Print process info
            std::cout << std::left
                << std::setw(6) << p->getPID()
                << std::setw(12) << name
                << std::setw(28) << p->getCreationTime()
                << std::setw(6) << p->getCoreID()
                << std::setw(12) << stateStr
                << std::to_string(p->getCurrentInstructionIndex()) + "/" + std::to_string(p->getTotalInstructions())
                << "\n";
        }
    }
    // If no running processes, notify user
    if (!anyRunning)
        CU::printColoredText(Color::Yellow, "[!] No running processes.\n");

    // Print finished processes header
    CU::printColoredText(Color::Blue, "\nFinished Processes\n");
    std::cout << "--------------------------------------------------------------------\n";
    std::cout << std::left
        << std::setw(6) << "PID"
        << std::setw(12) << "Name"
        << std::setw(28) << "Created"
        << std::setw(6) << "Core"
        << std::setw(12) << "Status"
        << std::setw(10) << "Instr.";
    std::cout << "\n--------------------------------------------------------------------\n";

    bool anyFinished = false;
    // List all finished or terminated processes
    for (const auto& p : processes) {
        if (p->isFinished() || p->isTerminated()) {
            anyFinished = true;
            std::string name = ConsoleUtil::truncateLongNames(p->getName());
            std::string stateStr;
            // Get process state as string
            switch (p->getState()) {
            case ProcessState::Finished: stateStr = "Finished"; break;
            case ProcessState::Terminated: stateStr = "Terminated"; break;
            }
            // Print process info
            std::cout << std::left
                << std::setw(6) << p->getPID()
                << std::setw(12) << name
                << std::setw(28) << p->getCreationTime()
                << std::setw(6) << p->getCoreID()
                << std::setw(12) << stateStr
                << std::to_string(p->getCurrentInstructionIndex()) + "/" + std::to_string(p->getTotalInstructions())
                << "\n";
        }
    }

    // If no finished processes, notify user
    if (!anyFinished)
        CU::printColoredText(Color::Yellow, "[!] No finished processes.\n");

    std::cout << std::endl;
}

void MainMenu::schedulerStart() {
    // If a test batch is already running, notify and return
    if (testingScheduler) {
        CU::printColoredText(Color::Yellow, "[!] Test batch is already running.\n");
        return;
    }

    // Set the flag to indicate the scheduler test is running
    testingScheduler = true;

    // Get system configuration
    const SystemConfig& config = ConsoleSystem::getInstance()->getConfig();
    // Use batchProcessFreq from config, default to 1 if zero
    unsigned long batchFreq = config.batchProcessFreq == 0 ? 1 : config.batchProcessFreq;
    const int maxProcesses = 20; // Maximum number of processes to generate

    // Launch a new thread for the test batch process generation
    testThread = std::thread([this, batchFreq, maxProcesses, config]() {
        int tick = 0;
        uint32_t totalAllocatedMemory = 0;

        // Continue while testingScheduler is true and process count is below max
        while (testingScheduler && Process::peakNextPID() < maxProcesses) {
            std::this_thread::sleep_for(TICK_PERIOD); // Wait for one tick
            ++tick;

            // Every batchFreq ticks, create a new process if name is not taken
            if (tick % batchFreq == 0) {
                std::string name = "Proc" + std::to_string(Process::peakNextPID());
                if (!ConsoleUtil::findProcessByName(name)) {
                    // Randomly determine memory required for the process
                    uint32_t memRequired = ConsoleUtil::rollBetween(
                        config.minMemoryPerProcess, 
                        config.maxMemoryPerProcess, 
                        config.memoryPerFrame
                    );

                    // Calculate number of pages needed
                    uint32_t pagesNeeded = (memRequired + config.memoryPerFrame - 1) / config.memoryPerFrame;

                    // Generate instructions for the process
                    auto instructions = InstructionGenerator::generateInstructions(
                        Process::peakNextPID(), config, memRequired
                    );

                    // Create the process object
                    auto newProcess = std::make_shared<Process>(name, instructions, memRequired, pagesNeeded);

                    // Allocate page table and register process
                    MemoryManager::getInstance()->allocatePageTable(newProcess);
                    GlobalScheduler::getInstance()->addProcess(newProcess);
                    Process::registerProcess(newProcess);

                    // Track total allocated memory (for possible future use)
                    totalAllocatedMemory += memRequired;
                }
            }
        }
    });

    // Notify user that the test process generator has started
    CU::printColoredText(Color::Green, "[*] Test process generator started.\n");
}


void MainMenu::schedulerStop() {
    // If the test scheduler is not running, notify the user and return
    if (!testingScheduler) {
        CU::printColoredText(Color::Red, "[X] No test process batch is currently running.\n");
        return;
    }

    // Set the flag to false to stop the test scheduler
    testingScheduler = false;

    // If the test thread is joinable, join it to clean up
    if (testThread.joinable())
        testThread.join();

    // Notify the user that the test process generator has stopped
    CU::printColoredText(Color::Green, "[*] Test process generator stopped.\n");
}


void MainMenu::reportUtil() {
    // Get scheduler, all processes, and CPU cores
    auto scheduler = GlobalScheduler::getInstance();
    auto processes = scheduler->getAllProcesses();
    auto cores = scheduler->getCores();

    // Get system configuration for CPU info
    const SystemConfig& config = ConsoleSystem::getInstance()->getConfig();
    int totalCores = config.numCPU;

    // Track which cores are currently used
    std::unordered_set<int> usedCores;
    for (const auto& core : cores) {
        if (!(core->isFree()))
            usedCores.insert(core->getId());
    }

    int coresUsed = static_cast<int>(usedCores.size());
    double cpuUtil = (static_cast<double>(coresUsed) / totalCores) * 100.0;

    // Prepare output stream for report
    std::ostringstream out;
    out << "=====================================================================\n";
    out << "CPU Utilization: " << std::fixed << std::setprecision(1) << cpuUtil << "%\n";
    out << "Cores Used:      " << coresUsed << "\n";
    out << "Cores Available: " << (totalCores - coresUsed) << "\n";
    out << "=====================================================================\n\n";

    // Print running processes header
    out << "Running Processes\n";
    out << "--------------------------------------------------------------------\n";
    out << std::left
        << std::setw(6) << "PID"
        << std::setw(12) << "Name"
        << std::setw(28) << "Created"
        << std::setw(6) << "Core"
        << std::setw(10) << "Status"
        << std::setw(10) << "Instr.";
    out << "\n--------------------------------------------------------------------\n";

    bool anyRunning = false;
    // List all running processes (not finished or terminated)
    for (const auto& p : processes) {
        if (!(p->isFinished() || p->isTerminated())) {
            anyRunning = true;
            std::string name = ConsoleUtil::truncateLongNames(p->getName());
            std::string stateStr;
            // Get process state as string
            switch (p->getState()) {
            case ProcessState::Blocked: stateStr = "Blocked"; break;
            case ProcessState::Ready: stateStr = "Ready"; break;
            case ProcessState::Running: stateStr = "Running"; break;
            case ProcessState::Sleeping: stateStr = "Sleeping"; break;
            }
            // Print process info
            out << std::left
                << std::setw(6) << p->getPID()
                << std::setw(12) << name
                << std::setw(28) << p->getCreationTime()
                << std::setw(6) << p->getCoreID()
                << std::setw(10) << stateStr
                << std::to_string(p->getCurrentInstructionIndex()) + "/" + std::to_string(p->getTotalInstructions())
                << "\n";
        }
    }
    // If no running processes, notify in report
    if (!anyRunning)
        out << "[!] No running processes.\n";

    // Print finished processes header
    out << "\n\nFinished Processes\n";
    out << "--------------------------------------------------------------------\n";
    out << std::left
        << std::setw(6) << "PID"
        << std::setw(12) << "Name"
        << std::setw(28) << "Created"
        << std::setw(6) << ""
        << std::setw(10) << "Status"
        << std::setw(10) << "Instr.";
    out << "\n--------------------------------------------------------------------\n";

    bool anyFinished = false;
    // List all finished or terminated processes
    for (const auto& p : processes) {
        if (p->isFinished() || p->isTerminated()) {
            anyFinished = true;
            std::string name = ConsoleUtil::truncateLongNames(p->getName());
            std::string stateStr;
            // Get process state as string
            switch (p->getState()) {
            case ProcessState::Finished: stateStr = "Finished"; break;
            case ProcessState::Terminated: stateStr = "Terminated"; break;
            }
            // Print process info
            out << std::left
                << std::setw(6) << p->getPID()
                << std::setw(12) << name
                << std::setw(28) << p->getCreationTime()
                << std::setw(6) << ""
                << std::setw(10) << stateStr
                << std::to_string(p->getCurrentInstructionIndex()) + "/" + std::to_string(p->getTotalInstructions())
                << "\n";
        }
    }
    // If no finished processes, notify in report
    if (!anyFinished)
        out << "[!] No finished processes.\n";

    // Write the report to a file
    std::ofstream fout("csopesy-log.txt");
    if (!fout.is_open()) {
        CU::printColoredText(Color::Red, "[X] Failed to write csopesy-log.txt\n");
        return;
    }

    fout << out.str();
    fout.close();

    // Notify user of successful report generation
    CU::printColoredText(Color::Green, "[*] CPU Utilization Report saved to csopesy-log.txt\n");
}

void MainMenu::processSMI() {
    // Get memory manager and scheduler instances
    auto mm = MemoryManager::getInstance();
    auto scheduler = GlobalScheduler::getInstance();
    const auto& config = ConsoleSystem::getInstance()->getConfig();

    // Count used frames in memory
    uint32_t usedFrames = 0;
    for (const auto& frame : mm->getFrameTable()) {
        if (frame.inUse) ++usedFrames;
    }

    // Calculate memory usage statistics
    uint32_t totalMemory = mm->getMemorySize();
    uint32_t usedMemory = usedFrames * mm->getFrameSize();
    float memoryUtil = (float)usedMemory / totalMemory * 100.0f;

    // Get CPU core usage
    auto cores = scheduler->getCores();
    int totalCores = config.numCPU;

    // Track which cores are currently used
    std::unordered_set<int> usedCores;
    for (const auto& core : cores) {
        if (!(core->isFree()))
            usedCores.insert(core->getId());
    }

    int coresUsed = static_cast<int>(usedCores.size());
    double cpuUtil = (static_cast<double>(coresUsed) / totalCores) * 100.0;

    // Prepare output stream for SMI report
    std::ostringstream out;
    out << "=====================================================================\n";
    out << "Process SMI - Memory Snapshot\n";
    out << "=====================================================================\n";
    out << "CPU Utilization: " << std::fixed << std::setprecision(1) << cpuUtil << "%\n";
    out << "Memory Usage: " << usedMemory << " / " << totalMemory << " bytes\n";
    out << "Memory Utilization: " << std::fixed << std::setprecision(2) << memoryUtil << "%\n";
    out << "=====================================================================\n";

    // Print table header
    out << std::left
        << std::setw(6) << "PID"
        << std::setw(12) << "Name"
        << std::setw(18) << "Memory Usage"
        << std::setw(7) << "%";
    out << "\n---------------------------------------------------------------------\n";

    // For each process, show memory usage
    for (const auto& process : scheduler->getAllProcesses()) {
        uint32_t pid = process->getPID();
        std::string name = ConsoleUtil::truncateLongNames(process->getName());
        uint32_t memRequired = process->getMemoryRequired();

        // Count frames used by this process
        uint32_t usedByProcess = 0;
        for (const auto& frame : mm->getFrameTable()) {
            if (frame.inUse && frame.pfid == pid) {
                ++usedByProcess;
            }
        }
        uint32_t actualUsedMemory = usedByProcess * mm->getFrameSize();
        float usagePercent = (memRequired == 0) ? 0.0f : (float)actualUsedMemory / memRequired * 100.0f;

        // Print process memory usage row
        out << std::left
            << std::setw(6) << process->getPID()
            << std::setw(12) << name
            << std::setw(18) << std::to_string(actualUsedMemory) + "/" + std::to_string(memRequired)
            << std::setw(7) << std::fixed << std::setprecision(2) << usagePercent << "%\n";
    }

    // Output the SMI report to the console
    std::cout << out.str();
}

void MainMenu::VMStat() {
    auto mm = MemoryManager::getInstance();
    auto scheduler = GlobalScheduler::getInstance();

    uint32_t usedFrames = 0;
    for (const auto& frame : mm->getFrameTable()) {
        if (frame.inUse) ++usedFrames;
    }

    uint32_t totalMemory = mm->getMemorySize();
    uint32_t usedMemory = usedFrames * mm->getFrameSize();
    uint32_t freeMemory = totalMemory - usedMemory;
    float memUtil = (float)usedMemory / totalMemory * 100.0f;

    size_t active = 0, inactive = 0;
    for (const auto& proc : scheduler->getAllProcesses()) {
        if (proc->getState() == ProcessState::Terminated) ++inactive;
        else ++active;
    }

    std::ostringstream out;
    out << "=====================================================================\n";
    out << "VMSTAT - Virtual Memory & CPU Stats\n";
    out << "=====================================================================\n";
    out << "Total Memory:      " << totalMemory << " bytes\n";
    out << "Used Memory:       " << usedMemory << " bytes\n";
    out << "Free Memory:       " << freeMemory << " bytes\n";
    out << "Memory Utilization: " << std::fixed << std::setprecision(2) << memUtil << "%\n";
    out << "---------------------------------------------------------------------\n";
    out << "Active Processes:   " << active << "\n";
    out << "Terminated:         " << inactive << "\n";
    out << "---------------------------------------------------------------------\n";
    out << "CPU Ticks (Idle):   " << scheduler->getIdleTicks() << "\n";
    out << "CPU Ticks (Active): " << scheduler->getActiveTicks() << "\n";
    out << "CPU Ticks (Total):  " << scheduler->getTotalTicks() << "\n";
    out << "---------------------------------------------------------------------\n";
    out << "Pages Paged In:     " << mm->getPagesPagedIn() << "\n";
    out << "Pages Paged Out:    " << mm->getPagesPagedOut() << "\n";
    out << "=====================================================================\n";

    std::cout << out.str();
}