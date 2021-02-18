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
