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

#include <libretro-common/features/features_cpu.h>
#include <libretro-common/libretro.h>

#include <functional>
#include <vector>

namespace brls
{

typedef retro_time_t Time;

/**
 * Returns the current CPU time in microseconds.
 */
inline Time getCPUTimeUsec()
{
    return cpu_features_get_time_usec();
}

typedef std::function<void()> TickingGenericCallback;

typedef std::function<void(bool)> TickingEndCallback;
typedef TickingGenericCallback TickingTickCallback;

// Interface representing something that "ticks" every frame for a certain amount of frames,
// like a timer, an animation, a background task...
// The library manages a list of running tickings. Each ticking is reponsible for managing its own
// lifetime by returning true or false in onUpdate.
class Ticking
{
  public:
    virtual ~Ticking();

    /**
     * Starts the ticking, pushing it to the list of running tickings.
     * If the ticking is finished, it will be restarted.
     * If the ticking is already running, this method will have no effect.
     */
    void start();

    /**
     * Stops the ticking if it was running, and executes the end callback.
     */
    void stop();

    /**
     * Sets a callback to be executed when the
     * ticking finishes.
     * The callback argument will be set to true if the ticking stopped
     * on its own, false if it was stopped early by the user.
     */
    void setEndCallback(TickingEndCallback endCallback);

    /**
     * Sets a callback to be executed at every tick
     * until the ticking finishes.
     *
     * The last animation tick will execute the tick callback
     * then the end callback.
     */
    void setTickCallback(TickingTickCallback tickCallback);

    /**
     * Returns true if the ticking is currently running.
     */
    bool isRunning();

    /**
     * Called internally by the main loop. Takes all running tickings
     * and updates them.
     */
    static void updateTickings();

    inline static std::vector<Ticking*> runningTickings;

  protected:
    /**
     * Executed every frame while the ticking lives.
     * Delta is the time difference in ms between the last frame
     * and the current one.
     * Must return false if the ticking is finished and should be
     * removed from the list of active tickings.
     * The end callback will automatically be called then.
     */
    virtual bool onUpdate(Time delta) = 0;

    /**
     * Called when the ticking becomes active.
     */
    virtual void onStart() {};

    /**
     * Called when the ticking is stopped, either by the user
     * or because it finished.
     */
    virtual void onStop() {};

  private:
    void stop(bool finished);

    bool running = false;

    TickingEndCallback endCallback   = [](bool finished) {};
    TickingTickCallback tickCallback = [] {};
};

// Represents a "finite" ticking that runs for a known amount of time
// and can be seek / reset / rewound.
class FiniteTicking : public Ticking
{
  public:
    /**
     * Rewinds the ticking to go back to the beginning
     * without losing its state (as opposed to reset() that clears
     * everything in the ticking).
     * Does not start or stop it.
     */
    void rewind();

    /**
     * Stops and resets the ticking, clearing its state
     * in the process (as opposed to rewind() that just restarts
     * the ticking from the beginning without losing the state).
     */
    void reset();

  protected:
    /**
     * Called when the ticking gets rewound.
     */
    virtual void onRewind() {};

    /**
     * Called when the ticking gets reset.
     */
    virtual void onReset() {};
};

} // namespace brls
