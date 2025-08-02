#pragma once
#include <chrono>
#include <semaphore>

#include "SystemConfig.h"

inline constexpr auto TICK_PERIOD = std::chrono::milliseconds(1000);