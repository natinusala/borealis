/*
    Copyright 2019 natinusala
    Copyright 2019 p-sam

    Licensed under the Apache License, Version 2.0 (the "License");
    you may not use this file except in compliance with the License.
    You may obtain a copy of the License at

        http://www.apache.org/licenses/LICENSE-2.0

    Unless required by applicable law or agreed to in writing, software
    distributed under the License is distributed on an "AS IS" BASIS,
    WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
    See the License for the specific language governing permissions and
    limitations under the License.
*/

#include <borealis/core/application.hpp>
#include <borealis/core/repeating_task.hpp>

namespace brls
{

RepeatingTask::RepeatingTask(Time interval)
    : interval(interval)
{
    Application::getTaskManager()->registerRepeatingTask(this);
}

void RepeatingTask::run(Time currentTime)
{
    this->lastRun = currentTime;
}

void RepeatingTask::start()
{
    this->onStart();
    this->running = true;
}

void RepeatingTask::pause()
{
    this->running = false;
}

void RepeatingTask::stop()
{
    this->pause();
    this->stopRequested = true;
}

void RepeatingTask::fireNow()
{
    if (!this->isRunning())
        return;

    Time currentTime = getCPUTimeUsec() / 1000;
    this->run(currentTime);
}

Time RepeatingTask::getInterval()
{
    return this->interval;
}

Time RepeatingTask::getLastRun()
{
    return this->lastRun;
}

bool RepeatingTask::isRunning()
{
    return this->running;
}

bool RepeatingTask::isStopRequested()
{
    return this->stopRequested;
}

RepeatingTask::~RepeatingTask()
{
    // Nothing to do here, only the Task Manager is supposed to free tasks
}

} // namespace brls
