#include "Console.h"
#include <Windows.h>
#include "ColorUtils.h"
#include "ConsoleLayout.h"

#include "Process.h"
#include "FCFSScheduler.h"
#include <iostream>
#include <iomanip>

int main() {
    ConsoleSystem console;

    SetConsoleOutputCP(CP_UTF8);

    console.initialize();
    console.run();






	// Scratch code for testing the console system and process management

	//ConsoleSystem* system = new ConsoleSystem();

 //   using Color = ColorUtils::Color;
 //   namespace CU = ColorUtils;

	//std::shared_ptr<Process> process1 = std::make_shared<Process>("Process10101", 100);
	//std::shared_ptr<Process> process2 = std::make_shared<Process>("Process2", 200);
	//std::shared_ptr<Process> process3 = std::make_shared<Process>("Process3", 300);
	//std::shared_ptr<Process> process4 = std::make_shared<Process>("Process4", 400);
	//std::shared_ptr<Process> process5 = std::make_shared<Process>("Process5", 500);
 //   system->initialize();
 //   system->getScheduler()->start();
 //   system->getScheduler()->addProcess(process5);
	//system->getScheduler()->addProcess(process1);
	//system->getScheduler()->addProcess(process2);
	//system->getScheduler()->addProcess(process3);
	//system->getScheduler()->addProcess(process4);

 //   while (true)
 //   {
 //       std::this_thread::sleep_for(std::chrono::seconds(3)); // Simulate some processing time

	//	// clear the console
	//	system->clearScreen();

 //       {
 //           std::cout << "-----------------------------------------------------------------" << std::endl;

 //           const std::vector<std::shared_ptr<Process>> allProcesses = system->getScheduler()->getAllProcesses();
 //            
 //           if (allProcesses.empty()) {
 //               CU::printColoredText(Color::Yellow, "No processes found.\n");
 //           }
 //           else {
 //               CU::printColoredText(Color::Aqua, "Running processes:\n");
 //               if (!(system->getScheduler()->allCoresFree())) {
 //                   for (const std::shared_ptr<Process> proc : allProcesses) {
 //                       if (proc->isComplete()) continue; // Skip finished processes

 //                       // ——— truncate long names ———
	//					std::string name = truncateLongNames(proc->getName());

 //                       std::cout << std::left
 //                           << std::setw(12) << name
 //                           << std::setw(27) << proc->getTimestamp();
 //                           
	//					if (proc->getCoreID() != -1) { // If the process is assigned to a core
 //                           std::cout << std::left << std::setw(6) << "Core: "
 //                               << std::setw(7) << proc->getCoreID();
 //                       }
 //                       else {
 //                           // If the process is not assigned to a core
 //                           std::cout << std::left << std::setw(13) << "Ready";
 //                       }

 //                       std::cout << std::left 
 //                           << (std::to_string(proc->getTotalInstructions() - proc->getRemainingInstruction()) + " / " + std::to_string(proc->getTotalInstructions()))
 //                           << "\n";
 //                   }
 //               } else {
 //                   CU::printColoredText(Color::Green, "All processes are executed.\n");
	//			}

 //               CU::printColoredText(Color::Aqua, "\nFinished processes:\n");
 //               if (!(system->getScheduler()->noProcessFinished())){
 //                   for (const std::shared_ptr<Process> proc : allProcesses) {
 //                       if (!proc->isComplete()) continue; // Skip running processes

 //                       // ——— truncate long names ———
 //                       std::string name = truncateLongNames(proc->getName());

 //                       std::cout << std::left 
 //                           << std::setw(12) << name
 //                           << std::setw(27) << proc->getTimestamp()
	//						<< std::setw(13) << "Finished"
 //                           << (std::to_string(proc->getTotalInstructions() - proc->getRemainingInstruction()) + " / " + std::to_string(proc->getTotalInstructions()))
 //                           << "\n";
 //                   }
 //               }
 //               else {
 //                   CU::printColoredText(Color::Green, "No processes finished yet.\n");
 //               }
 //           }

 //           std::cout << "-----------------------------------------------------------------" << std::endl;
 //       }
 //   }


 //   system->exit();

    return 0;
}