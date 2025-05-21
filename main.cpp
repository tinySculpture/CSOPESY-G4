#include "Console.h"
#include <Windows.h>

int main() {
    ConsoleSystem console;

    SetConsoleOutputCP(CP_UTF8);

    console.initialize();
    console.run();
    return 0;
}