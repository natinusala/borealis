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

#include <borealis/core/time.hpp>

namespace brls
{

void Ticking::updateTickings()
{
    // Update time
    static Time previousTime = 0;

    Time currentTime = getCPUTimeUsec() / 1000;
    Time delta       = previousTime == 0 ? 0 : currentTime - previousTime;

    previousTime = currentTime;

    // Update every running ticking, kill them and execute cb if they are finished
    // We have to clone the running tickings list to avoid altering it while
    // in the for loop (so if another ticking is started in a callback or during onUpdate())
    std::vector<Ticking*> tickings(Ticking::runningTickings);

    for (Ticking* ticking : tickings)
    {
        bool run = ticking->onUpdate(delta);

        ticking->tickCallback();

        if (!run)
            ticking->stop(true); // will remove the ticking from Ticking::runningTickings
    }
}

void Ticking::start()
{
    if (this->running)
        return;

    Ticking::runningTickings.push_back(this);

    this->running = true;

    this->onStart();
}

void Ticking::stop()
{
    this->stop(false);
}

void Ticking::stop(bool finished)
{
    if (!this->running)
        return;

    for (size_t i = 0; i < Ticking::runningTickings.size(); i++)
    {
        if (Ticking::runningTickings[i] == this)
        {
            Ticking::runningTickings.erase(Ticking::runningTickings.begin() + i);
            break;
        }
    }

    this->running = false;

    this->endCallback(finished);

    this->onStop();
}

void Ticking::setEndCallback(TickingEndCallback endCallback)
{
    this->endCallback = endCallback;
}

void Ticking::setTickCallback(TickingTickCallback tickCallback)
{
    this->tickCallback = tickCallback;
}

bool Ticking::isRunning()
{
    return this->running;
}

Ticking::~Ticking()
{
    this->stop();
}

void FiniteTicking::rewind()
{
    this->onRewind();
}

void FiniteTicking::reset()
{
    this->stop();
    this->onReset();
}

} // namespace brls
