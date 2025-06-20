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

    return 0;
}