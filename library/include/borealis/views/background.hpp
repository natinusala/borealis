/*
    Copyright 2020-2021 natinusala

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

#include <borealis/core/view.hpp>

namespace brls
{

// A special kind of view that has additional callbacks around
// the application frame() method (before and after the nanovg frame)
// An application can only have one background
class Background : public View
{
  public:
    /**
      * Called before the nanovg frame starts
      */
    virtual void preFrame() = 0;

    /**
     * Called after the end of the nanovg frame
     */
    virtual void postFrame() = 0;
};

} // namespace brls
