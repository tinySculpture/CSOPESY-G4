#include "Console.h"
#include <Windows.h>

#include "Process.h"
#include "FCFSScheduler.h"

int main() {
    /*
    ConsoleSystem console;

    SetConsoleOutputCP(CP_UTF8);

    console.initialize();
    console.run();
    */

	Process process1("Process1", 10);
	Process process2("Process2", 5);
	Process process3("Process3", 8);
	Process process4("Process4", 3);
	Process process5("Process5", 6);
    FCFSScheduler scheduler(3);
	scheduler.start();
	scheduler.addProcess(&process5);
	scheduler.addProcess(&process1);
	std::this_thread::sleep_for(std::chrono::seconds(10));
	scheduler.addProcess(&process2);
	scheduler.addProcess(&process3);
	scheduler.addProcess(&process4);
	std::this_thread::sleep_for(std::chrono::seconds(10));
	scheduler.stop();

    return 0;
}