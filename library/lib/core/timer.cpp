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

#include <borealis/core/timer.hpp>

namespace brls
{

void Timer::start(Time duration)
{
    this->duration = duration;
    this->start();
}

void Timer::start()
{
    Ticking::start();
}

void Timer::setDuration(Time duration)
{
    this->duration = duration;
}

void Timer::onStart()
{
    this->progress = 0;
}

bool Timer::onUpdate(Time delta)
{
    this->progress += delta;
    return this->progress < this->duration;
}

void Timer::onReset()
{
    this->progress = 0;
    this->duration = 0;
}

void Timer::onRewind()
{
    this->progress = 0;
}

void RepeatingTimer::start(Time period)
{
    this->period = period;
    this->start();
}

void RepeatingTimer::start()
{
    Ticking::start();
}

void RepeatingTimer::setPeriod(Time period)
{
    this->period = period;
}

void RepeatingTimer::setCallback(TickingGenericCallback callback)
{
    this->callback = callback;
}

void RepeatingTimer::onStart()
{
    this->progress = 0;
}

bool RepeatingTimer::onUpdate(Time delta)
{
    this->progress += delta;

    if (this->progress >= this->period)
    {
        this->callback();
        this->period = 0;
    }

    return true; // never stop
}

} // namespace brls
