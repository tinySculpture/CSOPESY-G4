#pragma once  
#include <map>  
#include <memory>  
#include <string>  
#include <vector>  

#include "Scheduler.h"  
#include "Process.h"  

class GlobalScheduler  
{  
private:  
    GlobalScheduler(); // Constructor declaration without return type  
    ~GlobalScheduler() = default;  
    GlobalScheduler(GlobalScheduler const&) {};  
    GlobalScheduler& operator=(GlobalScheduler const&) {};  
    static GlobalScheduler* sharedInstance;  

    std::map<std::string, std::shared_ptr<Scheduler>> schedulers;  
    std::shared_ptr<Scheduler> currentScheduler;  

    bool running = false;  

public:  
    static GlobalScheduler* getInstance();  
    static void initialize();  
    static void destroy();  

    void start();  
    void stop();  

    void setCurrentScheduler(const std::string name);  

    void addProcess(std::shared_ptr<Process> process);  

    std::vector<std::shared_ptr<Process>> getAllProcesses() const;  

    bool isRunning();  

    bool allCoresFree() const;  
    bool noProcessFinished() const;  
};