#include <Windows.h>

#include "Console.h"

int main() {
    SetConsoleOutputCP(CP_UTF8);

    ConsoleSystem::initialize();

    bool isRunning = true;
    while (isRunning) {
        ConsoleSystem::getInstance()->drawConsole();
        ConsoleSystem::getInstance()->processInput();

        isRunning = ConsoleSystem::getInstance()->isRunning();
    }

    ConsoleSystem::destroy();

    return 0;
}