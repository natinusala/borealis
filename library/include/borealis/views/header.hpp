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

#pragma once

#include <borealis/core/box.hpp>
#include <borealis/views/label.hpp>

namespace brls
{

// A simple header with a title, an optional subtitle, a rectangle on the left
// and a separator
class Header : public Box
{
  public:
    Header();

    void setTitle(std::string text);
    void setSubtitle(std::string text);

    static View* create();

  private:
    Label* title;
    Label* subtitle;
};

} // namespace brls
