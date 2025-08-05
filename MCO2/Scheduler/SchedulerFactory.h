#pragma once
#include "Scheduler.h"
#include "FCFSScheduler.h"
#include "RRScheduler.h"
#include "../Config/Config.h"
#include <memory>

std::unique_ptr<Scheduler> createSchedulerFromConfig(const Config& config, ScreenManager& sm);
