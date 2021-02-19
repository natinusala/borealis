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
        this->progress = 0;
    }

    return true; // never stop
}

} // namespace brls
