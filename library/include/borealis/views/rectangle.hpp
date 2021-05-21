/*
    Copyright 2020-2021 natinusala
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

#include <borealis/core/frame_context.hpp>
#include <borealis/core/view.hpp>

namespace brls
{

// A solid color rectangle
class Rectangle : public View
{
  public:
    Rectangle(NVGcolor color);
    Rectangle();
    ~Rectangle() { }

    void draw(NVGcontext* vg, float x, float y, float width, float height, Style style, FrameContext* ctx) override;

    void setColor(NVGcolor color);

    static View* create();

  private:
    NVGcolor color = nvgRGB(0, 0, 255);
};

} // namespace brls
