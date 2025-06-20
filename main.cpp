#include "Console.h"
#include "GlobalScheduler.h"
#include "ColorUtils.h"
#include "ConsoleLayout.h"

#include <Windows.h>
#include <iostream>
#include <iomanip>

int main() {
    SetConsoleOutputCP(CP_UTF8);

    ConsoleSystem::initialize();
	GlobalScheduler::initialize();

	bool isRunning = true;
    while (isRunning) {
        ConsoleSystem::getInstance()->drawConsole();
        ConsoleSystem::getInstance()->processInput();

		isRunning = ConsoleSystem::getInstance()->isRunning();
	}

	ConsoleSystem::destroy();
	GlobalScheduler::destroy();

    return 0;
}