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

#include <borealis/core/animation.hpp>
#include <vector>

namespace brls
{

Animatable::Animatable(float value)
    : currentValue(value)
{
}

void Animatable::onReset()
{
    this->tween = tweeny::tween<float>::from(this->currentValue);
}

void Animatable::reset(float initialValue)
{
    this->currentValue = initialValue;
    FiniteTicking::reset();
}

void Animatable::reset()
{
    FiniteTicking::reset();
}

void Animatable::onRewind()
{
    this->currentValue = this->tween.seek(0);
}

void Animatable::addStep(float targetValue, int32_t duration, EasingFunction easing)
{
    this->tween.to(targetValue).during(duration).via(easing);
}

float Animatable::getProgress()
{
    return this->tween.progress();
}

bool Animatable::onUpdate(retro_time_t delta)
{
    // int32_t for stepping works as long as the app goes faster than 0.00001396983 FPS
    // (in which case the delta for a frame wraps in an int32_t)
    this->currentValue = this->tween.step((int32_t)delta);
    return this->tween.progress() < 1.0f;
}

float Animatable::getValue()
{
    return this->currentValue;
}

Animatable::operator float() const
{
    return this->currentValue;
}

Animatable::operator float()
{
    return this->currentValue;
}

void Animatable::operator=(const float value)
{
    this->reset(value);
}

bool Animatable::operator==(const float value)
{
    return this->currentValue == value;
}

#define HIGHLIGHT_SPEED 125.0f

static double highlightGradientX = 0;
static double highlightGradientY = 0;
static double highlightColor     = 0;

void updateHighlightAnimation()
{
    Time currentTime = getCPUTimeUsec() / 1000;

    // Update variables
    highlightGradientX = (cos((double)currentTime / HIGHLIGHT_SPEED / 3.0) + 1.0) / 2.0;
    highlightGradientY = (sin((double)currentTime / HIGHLIGHT_SPEED / 3.0) + 1.0) / 2.0;
    highlightColor     = (sin((double)currentTime / HIGHLIGHT_SPEED * 2.0) + 1.0) / 2.0;
}

void getHighlightAnimation(float* gradientX, float* gradientY, float* color)
{
    if (gradientX)
        *gradientX = (float)highlightGradientX;

    if (gradientY)
        *gradientY = (float)highlightGradientY;

    if (color)
        *color = (float)highlightColor;
}

} // namespace brls
