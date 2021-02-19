/*
    Copyright 2021 natinusala

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

#pragma once

#include <borealis/core/timer.hpp>

namespace brls
{

// A RepeatingTask is a task executed repeatedly on the main thread at a given period in ms
class RepeatingTask : private RepeatingTimer
{
  public:
    /**
     * Create a RepeatingTask with the given period in ms.
     *
     * The task is guaranteed to wait for at least the given period of time between
     * handler runs, but it can wait for slightly longer (usually less than a ms late)
     */
    RepeatingTask(Time period);

    /**
     * Task handler executed repeatedly on the main thread at the given period.
     */
    virtual void run() = 0;

    /**
     * Starts the task.
     */
    void start();

    /**
     * Stops the task.
     */
    void stop();
};
} // namespace brls
