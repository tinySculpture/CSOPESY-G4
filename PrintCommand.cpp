#include "PrintCommand.h"

PrintCommand::PrintCommand(int pid, const std::string message)
	: ICommand(pid, PRINT), toPrint(message) {}

void PrintCommand::execute() {
    // Only print if this process is currently active in the console
    ConsoleSystem* sys = ConsoleSystem::getInstance();
    if (sys->getCurrentProcessId() != pid) {
        return;
    }

    // Output the stored message
    std::cout << toPrint << std::endl;
}