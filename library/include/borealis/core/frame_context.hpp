/*
    Copyright 2019 natinusala
    Copyright 2019 psam

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

#include <nanovg.h>

#include <borealis/core/font.hpp>
#include <borealis/core/style.hpp>
#include <borealis/core/theme.hpp>

namespace brls
{

class FrameContext
{
  public:
    NVGcontext* vg       = nullptr;
    float pixelRatio     = 0.0;
    FontStash* fontStash = nullptr;
    Theme theme;
};

} // namespace brls
