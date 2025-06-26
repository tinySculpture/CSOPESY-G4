#include <ctime>
#include <iomanip>
#include <iostream>
#include <random>
#include <sstream>

#include "GlobalScheduler.h"
#include "ConsoleUtil.h"

std::vector<std::string> ConsoleUtil::tokenizeInput(const std::string input) {
    std::vector<std::string> tokens;
    std::istringstream stream(input);
    std::string token;

    while (stream >> token) {
        tokens.push_back(token);
    }

    return tokens;
}

std::shared_ptr<Process> ConsoleUtil::findProcessByName(const std::string& name) {
    const auto all = GlobalScheduler::getInstance()->getAllProcesses();
    for (const auto& p : all) {
        if (p && p->getName() == name) {
            return p;
        }
    }
    return nullptr;
}

std::string ConsoleUtil::generateTimestamp() {
    std::ostringstream oss;

    std::time_t t = std::time(nullptr);

    std::tm now;

    localtime_s(&now, &t);

    oss << std::put_time(&now, "(%m/%d/%Y %I:%M:%S%p)");

    return oss.str();
}

std::string ConsoleUtil::truncateLongNames(std::string name) {
    if (name.size() > 9) {
        const size_t maxDisplay = 9;
        const size_t visible = maxDisplay - 3;
        name = "..." + name.substr(name.size() - visible);
    }

    return name;
}