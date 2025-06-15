#include "Console.h"
#include <Windows.h>

#include "Process.h"
#include "FCFSScheduler.h"

int main() {
    ConsoleSystem console;

    SetConsoleOutputCP(CP_UTF8);

    console.initialize();
    console.run();

	/*std::shared_ptr<Process> process1 = std::make_shared<Process>("Process1", 1000);
	std::shared_ptr<Process> process2 = std::make_shared<Process>("Process2", 2000);
	std::shared_ptr<Process> process3 = std::make_shared<Process>("Process3", 3000);
	std::shared_ptr<Process> process4 = std::make_shared<Process>("Process4", 4000);
	std::shared_ptr<Process> process5 = std::make_shared<Process>("Process5", 5000);
    FCFSScheduler scheduler(3);
	scheduler.start();
	scheduler.addProcess(process5);
	scheduler.addProcess(process1);
	scheduler.addProcess(process2);
	scheduler.addProcess(process3);
	scheduler.addProcess(process4);
	std::this_thread::sleep_for(std::chrono::seconds(10));
	scheduler.stop();*/

    return 0;
}