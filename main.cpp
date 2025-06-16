#include "Console.h"
#include <Windows.h>
#include "ColorUtils.h"
#include "ConsoleLayout.h"

#include "Process.h"
#include "FCFSScheduler.h"
#include <iostream>
#include <iomanip>

int main() {
    /*ConsoleSystem console;

    SetConsoleOutputCP(CP_UTF8);

    console.initialize();
    console.run();*/

	ConsoleSystem* system = new ConsoleSystem();

    using Color = ColorUtils::Color;
    namespace CU = ColorUtils;

	std::shared_ptr<Process> process1 = std::make_shared<Process>("Process1", 100);
	std::shared_ptr<Process> process2 = std::make_shared<Process>("Process2", 200);
	std::shared_ptr<Process> process3 = std::make_shared<Process>("Process3", 300);
	std::shared_ptr<Process> process4 = std::make_shared<Process>("Process4", 400);
	std::shared_ptr<Process> process5 = std::make_shared<Process>("Process5", 500);
    system->initialize();
    system->getScheduler()->start();
    system->getScheduler()->addProcess(process5);
	system->getScheduler()->addProcess(process1);
	system->getScheduler()->addProcess(process2);
	system->getScheduler()->addProcess(process3);
	system->getScheduler()->addProcess(process4);

    while (true)
    {
        std::this_thread::sleep_for(std::chrono::seconds(3)); // Simulate some processing time

		// clear the console
		system->clearScreen();

        {
            const std::vector<std::shared_ptr<Process>> readyProcesses = system->getScheduler()->getReadyQueue();
            const std::vector<std::shared_ptr<Process>> finishedProcesses = system->getScheduler()->getFinishedQueue();
            const std::vector<std::shared_ptr<Process>> runningProcesses = system->getScheduler()->getCoreProcesses();
             
            if (readyProcesses.empty() && runningProcesses.empty() && finishedProcesses.empty()) {
                CU::printColoredText(Color::Yellow, "No processes found.\n");
            }
            else {
                CU::printColoredText(Color::Aqua, "\nReady Process List:\n");
                std::cout << std::left << std::setw(20) << "Name"
                    << std::setw(15) << "Current/Total"
                    << "Timestamp" << "\n";

                for (const std::shared_ptr<Process> proc : readyProcesses) {
                    std::cout << std::left << std::setw(20) << proc->getName()
                        << std::setw(15) << (std::to_string(proc->getTotalInstructions() - proc->getRemainingInstruction()) + "/" + std::to_string(proc->getTotalInstructions()))
                        << proc->getTimestamp() << "\n";
                }

                CU::printColoredText(Color::Aqua, "\nRunning Process List:\n");
                std::cout << std::left << std::setw(20) << "Name"
                    << std::setw(15) << "Current/Total"
                    << "Timestamp" << "\n";

                for (const std::shared_ptr<Process> proc : runningProcesses) {
                    if (proc == nullptr) continue; // Skip null pointers
                    std::cout << std::left << std::setw(20) << proc->getName()
                        << std::setw(15) << (std::to_string(proc->getTotalInstructions() - proc->getRemainingInstruction()) + "/" + std::to_string(proc->getTotalInstructions()))
                        << proc->getTimestamp() << "\n";
                }

                CU::printColoredText(Color::Aqua, "\n\nFinished Process List:\n");
                std::cout << std::left << std::setw(20) << "Name"
                    << std::setw(15) << "Current/Total"
                    << "Timestamp" << "\n";

                for (const std::shared_ptr<Process> proc : finishedProcesses) {
                    std::cout << std::left << std::setw(20) << proc->getName()
                        << std::setw(15) << (std::to_string(proc->getTotalInstructions() - proc->getRemainingInstruction()) + "/" + std::to_string(proc->getTotalInstructions()))
                        << proc->getTimestamp() << "\n";
                }
            }
        }
    }


    system->exit();

    return 0;
}