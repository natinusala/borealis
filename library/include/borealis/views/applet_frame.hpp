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

#include <borealis/core/bind.hpp>
#include <borealis/core/box.hpp>
#include <borealis/views/image.hpp>
#include <borealis/views/label.hpp>

namespace brls
{

// A Horizon settings-like frame, with header and footer (no sidebar)
class AppletFrame : public Box
{
  public:
    AppletFrame();

    void handleXMLElement(tinyxml2::XMLElement* element) override;

    /**
     * Sets the content view for that AppletFrame.
     * Will be placed between header and footer and expanded with grow factor
     * and width / height to AUTO.
     */
    void setContentView(View* view);

    void setTitle(std::string title);

    void setIconFromRes(std::string name);
    void setIconFromFile(std::string path);

    static View* create();

  private:
    BRLS_BIND(Label, title, "brls/applet_frame/title_label");
    BRLS_BIND(Image, icon, "brls/applet_frame/title_icon");

  protected:
    View* contentView = nullptr;
};

} // namespace brls
