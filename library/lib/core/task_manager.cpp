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

#include <borealis/core/task_manager.hpp>

namespace brls
{

void TaskManager::frame()
{
    // Repeating tasks
    Time currentTime = getCPUTimeUsec() / 1000;
    for (auto i = this->repeatingTasks.begin(); i != this->repeatingTasks.end(); i++)
    {
        RepeatingTask* task = *i;

        // Stop the task if needed
        if (task->isStopRequested())
        {
            this->stopRepeatingTask(task);
            this->repeatingTasks.erase(i--);
        }
        // Fire it
        else if (task->isRunning() && currentTime - task->getLastRun() > task->getInterval())
        {
            task->run(currentTime);
        }
    }
}

void TaskManager::registerRepeatingTask(RepeatingTask* task)
{
    this->repeatingTasks.push_back(task);
}

void TaskManager::stopRepeatingTask(RepeatingTask* task)
{
    task->onStop();
    delete task;
}

TaskManager::~TaskManager()
{
    // Stop all repeating tasks
    for (RepeatingTask* task : this->repeatingTasks)
        this->stopRepeatingTask(task);

    this->repeatingTasks.clear();
}

} // namespace brls
