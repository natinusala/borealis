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

#include <borealis/core/time.hpp>

namespace brls
{

// A Timer allows to run a callback once after a given period of time, in ms
// Add the callback with setEndCallback(), set the duration with setDuration() then start the timer
class Timer : public FiniteTicking
{
  public:
    /**
     * Starts the timer directly with a given duration, in ms.
     */
    void start(Time duration);

    /**
     * Starts the timer with the already set duration.
     */
    void start();

    /**
     * Sets the duration of the timer, in ms.
     * Does not stop or reset it.
     */
    void setDuration(Time duration);

    void onStart() override;
    bool onUpdate(Time delta) override;
    void onReset() override;
    void onRewind() override;

  protected:
    Time duration = 0;
    Time progress = 0;
};

// A RepeatingTimer allows to run a callback repeatedly at a given time interval, in ms
// Add the callback with setCallback(), set the period with setPeriod() then start the timer
class RepeatingTimer : public Ticking
{
  public:
    /**
     * Starts the timer directly with a given period, in ms.
     */
    void start(Time period);

    /**
     * Starts the timer with the already set period.
     */
    void start();

    /**
     * Sets the period of the timer, in ms.
     * Does not stop or reset it.
     */
    void setPeriod(Time period);

    /**
     * Sets the callback of the timer.
     * Tick callback is still executed every tick.
     * End callback is executed when the timer is stopped.
     */
    void setCallback(TickingGenericCallback callback);

    void onStart() override;
    bool onUpdate(Time delta) override;

  protected:
    Time period   = 0;
    Time progress = 0;

    TickingGenericCallback callback = [] {};
};

} // namespace brls
