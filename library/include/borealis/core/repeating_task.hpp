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

#pragma once

#include <borealis/core/time.hpp>

namespace brls
{

// A task that is repeated at a given interval
// by the UI thread
class RepeatingTask
{
  private:
    Time interval;

    Time lastRun = 0;

    bool running       = false;
    bool stopRequested = false;

  public:
    RepeatingTask(Time interval);
    virtual ~RepeatingTask();

    /**
      * Actual code to run by the task
      * Must call RepeatingTask::run() !
      */
    virtual void run(Time currentTime);

    /**
      * Fired when the task starts
      */
    virtual void onStart() {};

    /**
      * Fired when the task stops
      */
    virtual void onStop() {};

    /**
      * Starts the task
      */
    void start();

    /**
      * Fires the task immediately and delays the
      * next run
      */
    void fireNow();

    /**
      * Pauses the task without deleting it
      */
    void pause();

    /**
      * Stops and deletes the task
      */
    void stop();

    Time getInterval();
    Time getLastRun();

    bool isRunning();
    bool isStopRequested();
};

} // namespace brls
