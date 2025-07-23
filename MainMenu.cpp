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
#include "Globals.h"

using Color = ColorUtil::Color;
namespace CU = ColorUtil;

std::thread MainMenu::testThread;
std::atomic<bool> MainMenu::testingScheduler{ false };

MainMenu::MainMenu()
    : ConsoleLayout("MainMenu") {
}

void MainMenu::render() {
    std::cout << "---------------------------------------------" << std::endl;
    std::cout << "  _____ _____ _____ _____ _____ _____ __ __  " << std::endl <<
                 " |     |   __|     |  _  |   __|   __|  |  | " << std::endl <<
                 " |   --|__   |  |  |   __|   __|__   |_   _| " << std::endl <<
                 " |_____|_____|_____|__|  |_____|_____| |_|   " << "\n" << std::endl;
    std::cout << "---------------------------------------------" << std::endl;

    CU::printColoredText(Color::Green, "\nWelcome to the CSOPESY Emulator!\n");

    std::cout << "Developers:" << std::endl;
    std::cout << "Luistro, Josiah Mari" << std::endl;
    std::cout << "Rimando, Alonzo Andrei" << std::endl;
    std::cout << "Santos, Andrea Li" << std::endl;
    std::cout << "Vasquez, Ryan Clemence" << std::endl;
    std::cout << "\nLast Updated: ";

    CU::printColoredText(Color::Green, "25 - 6 - 2025\n");
    std::cout << "---------------------------------------------" << std::endl;
}

bool MainMenu::processInput(const std::string input) {
    std::vector<std::string> tokens = ConsoleUtil::tokenizeInput(input);

    if (tokens.empty()) {
    CU::printColoredText(Color::Red, "[X] Error: Empty input.\n");
        return false;
    }

    const std::string& command = tokens[0];

    if (command == "exit") {
        if (ConsoleSystem::getInstance()->isInitialized()) {
            CU::printColoredText(Color::Yellow, "[!] Exiting the system. Please wait...\n");
            GlobalScheduler::getInstance()->stop();
            ConsoleSystem::getInstance()->exit();
        }
        ConsoleSystem::getInstance()->exit();
    }
    else if (command == "clear") {
        ConsoleSystem::getInstance()->switchLayout("MainMenu");
        ConsoleSystem::getInstance()->drawConsole();
    }
    else if (command == "initialize") {
        if (ConsoleSystem::getInstance()->isInitialized()) {
            CU::printColoredText(Color::Red, "[X] System is already initialized.\n");
            return false;
        }
        ConsoleSystem::getInstance()->configure("config.txt");
        GlobalScheduler::getInstance()->start();
        GlobalMemoryAllocator::getInstance()->start();
    }
    else if (command == "scheduler-start" || command == "scheduler-stop" || command == "report-util" || command == "screen") {
        if (!ConsoleSystem::getInstance()->isInitialized()) {
            CU::printColoredText(Color::Red, "[X] Please initialize the system first.\n");
            return false;
        }

        if (command == "screen") {
            if (tokens.size() == 3 && tokens[1] == "-s") {
                screenStart(tokens[2]);
            }
            else if (tokens.size() == 3 && tokens[1] == "-r") {
                screenResume(tokens[2]);
            }
            else if (tokens.size() == 2 && tokens[1] == "-ls") {
                screenList();
            }
            else {
                CU::printColoredText(Color::Red, "[X] Proper Usage: screen -s|-r <name> or screen -ls.\n");
            }
        }
        else if (command == "scheduler-start") {
            schedulerStart();
        }
        else if (command == "scheduler-stop") {
            schedulerStop();
        }
        else if (command == "report-util") {
            reportUtil();
        }
    }
    else {
        CU::printColoredText(Color::Red, "[X] Invalid Command. Please Try Again.\n");
    }

    return false;
}

void MainMenu::screenStart(const std::string& process_name) {
    const SystemConfig& config = ConsoleSystem::getInstance()->getConfig();
    auto process = ConsoleUtil::findProcessByName(process_name);

    if (!process) {
        auto instructions = InstructionGenerator::generateInstructions(Process::peakNextPID(), config);
        auto newProcess = std::make_shared<Process>(process_name, instructions);

        GlobalScheduler::getInstance()->addProcess(newProcess);

        CU::printColoredText(Color::Green, "[*] Process \"" + process_name + "\" created successfully.\n");

        // ConsoleSystem::getInstance()->switchLayout("ProcessScreen", newProcess);
    }
    else {
        CU::printColoredText(Color::Red, "[X] Process \"" + process_name + "\" already exists.\n");
    }
}


void MainMenu::screenResume(const std::string& process_name) {
    auto process = ConsoleUtil::findProcessByName(process_name);

    if (!process) {
        CU::printColoredText(Color::Red, "[X] No such process found.\n");
        return;
    }

    ConsoleSystem::getInstance()->switchLayout("ProcessScreen", process);
}


void MainMenu::screenList() {
    auto scheduler = GlobalScheduler::getInstance();
    auto processes = scheduler->getAllProcesses();
    auto cores = scheduler->getCores();

    const SystemConfig& config = ConsoleSystem::getInstance()->getConfig();
    int totalCores = config.numCPU;

    std::unordered_set<int> usedCores;
    for (const auto& core : cores) {
        if (!(core->isFree()))
            usedCores.insert(core->getId());
    }

    int coresUsed = static_cast<int>(usedCores.size());
    double cpuUtil = (static_cast<double>(coresUsed) / totalCores) * 100.0;

    std::cout << "=====================================================================\n";
    std::cout << "CPU Utilization: " << std::fixed << std::setprecision(1) << cpuUtil << "%\n";
    std::cout << "Cores Used:      " << coresUsed << "\n";
    std::cout << "Cores Available: " << (totalCores - coresUsed) << "\n";
    std::cout << "=====================================================================\n\n";

    CU::printColoredText(Color::Blue, "Running Processes\n");
    std::cout << "---------------------------------------------------------------------\n";
    std::cout << std::left
        << std::setw(6) << "PID"
        << std::setw(12) << "Name"
        << std::setw(28) << "Created"
        << std::setw(6) << "Core"
        << std::setw(10) << "Status"
        << std::setw(10) << "Instr.";
    std::cout << "\n--------------------------------------------------------------------\n";

    bool anyRunning = false;
    for (const auto& p : processes) {
        if (!(p->isFinished())) {
            anyRunning = true;
            std::string name = ConsoleUtil::truncateLongNames(p->getName());
            std::string stateStr;
            switch (p->getState()) {
            case ProcessState::Ready: stateStr = "Ready"; break;
            case ProcessState::Running: stateStr = "Running"; break;
            case ProcessState::Sleeping: stateStr = "Sleeping"; break;
            }
            std::cout << std::left
                << std::setw(6) << p->getPID()
                << std::setw(12) << name
                << std::setw(28) << p->getCreationTime()
                << std::setw(6) << p->getCoreID()
                << std::setw(10) << stateStr
                << std::to_string(p->getCurrentInstructionIndex()) + "/" + std::to_string(p->getTotalInstructions())
                << "\n";
        }
    }
    if (!anyRunning)
        CU::printColoredText(Color::Yellow, "[!] No running processes.\n");

    CU::printColoredText(Color::Blue, "\nFinished Processes\n");
    std::cout << "--------------------------------------------------------------------\n";
    std::cout << std::left
        << std::setw(6) << "PID"
        << std::setw(12) << "Name"
        << std::setw(28) << "Created"
        << std::setw(6) << "Core"
        << std::setw(10) << "Status"
        << std::setw(10) << "Instr.";
    std::cout << "\n--------------------------------------------------------------------\n";

    bool anyFinished = false;
    for (const auto& p : processes) {
        if (p->isFinished()) {
            anyFinished = true;
            std::string name = ConsoleUtil::truncateLongNames(p->getName());
            std::cout << std::left
                << std::setw(6) << p->getPID()
                << std::setw(12) << name
                << std::setw(28) << p->getCreationTime()
				<< std::setw(6) << p->getCoreID()
                << std::setw(10) << "Finished"
                << std::to_string(p->getCurrentInstructionIndex()) + "/" + std::to_string(p->getTotalInstructions())
                << "\n";
        }
    }

    if (!anyFinished)
        CU::printColoredText(Color::Yellow, "[!] No finished processes.\n");

    std::cout << std::endl;
}

void MainMenu::schedulerStart() {
    if (testingScheduler) {
        CU::printColoredText(Color::Yellow, "[!] Test batch is already running.\n");
        return;
    }

    testingScheduler = true;

    const SystemConfig& config = ConsoleSystem::getInstance()->getConfig();
    unsigned long batchFreq = config.batchProcessFreq == 0 ? 1 : config.batchProcessFreq;
    const int maxProcesses = 20;

    testThread = std::thread([this, batchFreq, maxProcesses, config]() {
        int tick = 0;
        while (testingScheduler && Process::peakNextPID() < maxProcesses) {
            std::this_thread::sleep_for(TICK_PERIOD);
            ++tick;

            if (tick % batchFreq == 0) {
                std::string name = "Proc" + std::to_string(Process::peakNextPID());
                if (!ConsoleUtil::findProcessByName(name)) {
                    auto instructions = InstructionGenerator::generateInstructions(Process::peakNextPID(), config);
                    auto newProcess = std::make_shared<Process>(name, instructions);
                    GlobalScheduler::getInstance()->addProcess(newProcess);
                }
            }
        }
        });

    CU::printColoredText(Color::Green, "[*] Test process generator started.\n");
}


void MainMenu::schedulerStop() {
    if (!testingScheduler) {
        CU::printColoredText(Color::Red, "[X] No test process batch is currently running.\n");
        return;
    }

    testingScheduler = false;

    if (testThread.joinable())
        testThread.join();

    CU::printColoredText(Color::Green, "[*] Test process generator stopped.\n");
}


void MainMenu::reportUtil() {
    auto scheduler = GlobalScheduler::getInstance();

    const SystemConfig& config = ConsoleSystem::getInstance()->getConfig();
    int totalCores = config.numCPU;

    auto processes = scheduler->getAllProcesses();
    auto cores = scheduler->getCores();
    std::unordered_set<int> usedCores;

    for (const auto& core : cores) {
        if (!(core->isFree()))
            usedCores.insert(core->getId());
    }

    int coresUsed = static_cast<int>(usedCores.size());
    double cpuUtil = (static_cast<double>(coresUsed) / totalCores) * 100.0;

    std::ostringstream out;
    out << "=====================================================================\n";
    out << "CPU Utilization: " << std::fixed << std::setprecision(1) << cpuUtil << "%\n";
    out << "Cores Used:      " << coresUsed << "\n";
    out << "Cores Available: " << (totalCores - coresUsed) << "\n";
    out << "=====================================================================\n\n";

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
    for (const auto& p : processes) {
        if (!(p->isFinished())) {
            anyRunning = true;
            std::string name = ConsoleUtil::truncateLongNames(p->getName());
            std::string stateStr;
            switch (p->getState()) {
            case ProcessState::Ready: stateStr = "Ready"; break;
            case ProcessState::Running: stateStr = "Running"; break;
            case ProcessState::Sleeping: stateStr = "Sleeping"; break;
            }
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
    if (!anyRunning)
        out << "[!] No running processes.\n";

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
    for (const auto& p : processes) {
        if (p->isFinished()) {
            anyFinished = true;
            std::string name = ConsoleUtil::truncateLongNames(p->getName());
            out << std::left
                << std::setw(6) << p->getPID()
                << std::setw(12) << name
                << std::setw(28) << p->getCreationTime()
                << std::setw(6) << ""
                << std::setw(10) << "Finished"
                << std::to_string(p->getCurrentInstructionIndex()) + "/" + std::to_string(p->getTotalInstructions())
                << "\n";
        }
    }
    if (!anyFinished)
        out << "[!] No finished processes.\n";


    std::ofstream fout("csopesy-log.txt");
    if (!fout.is_open()) {
        CU::printColoredText(Color::Red, "[X] Failed to write csopesy-log.txt\n");
        return;
    }

    fout << out.str();
    fout.close();

    CU::printColoredText(Color::Green, "[*] CPU Utilization Report saved to csopesy-log.txt\n");
}