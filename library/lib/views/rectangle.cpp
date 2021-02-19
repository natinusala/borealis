/*
    Copyright 2019-2021 natinusala
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

#include <borealis/core/logger.hpp>
#include <borealis/views/rectangle.hpp>

namespace brls
{

Rectangle::Rectangle(NVGcolor color)
{
    this->setColor(color);

    // Register XML attributes
    this->registerColorXMLAttribute("color", [this](NVGcolor color) {
        this->setColor(color);
    });
}

Rectangle::Rectangle()
    : Rectangle(nvgRGB(255, 255, 255))
{
    // Empty ctor for XML
}

void Rectangle::draw(NVGcontext* vg, float x, float y, float width, float height, Style style, FrameContext* ctx)
{
    NVGcolor color = a(this->color);

    if (color.a == 0.0f)
        return;

    nvgFillColor(vg, color);

    nvgBeginPath(vg);
    nvgRect(vg, x, y, width, height);
    nvgFill(vg);
}

void Rectangle::setColor(NVGcolor color)
{
    this->color = color;
}

// void Rectangle::layout(NVGcontext* vg, Style* style, FontStash* stash)
// {
//     // Nothing to do
// }

View* Rectangle::create()
{
    return new Rectangle();
}

} // namespace brls
