/*
    Borealis, a Nintendo Switch UI Library
    Copyright (C) 2021  natinusala

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <https://www.gnu.org/licenses/>.
*/

#pragma once

#include <borealis/core/time.hpp>

namespace brls
{

// TODO: rewrite RepeatingTask to just use a RepeatingTimer internally

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

// A RepeatingTimer allows to run a callback repeatidly at a given time interval, in ms
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
