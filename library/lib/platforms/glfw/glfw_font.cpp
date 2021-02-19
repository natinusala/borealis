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

#include <unistd.h>

#include <borealis/core/application.hpp>
#include <borealis/core/assets.hpp>
#include <borealis/platforms/glfw/glfw_font.hpp>

#define USER_REGULAR_PATH BRLS_ASSET("User-Regular.ttf")
#define INTER_FONT_PATH BRLS_ASSET("inter/Inter-Switch.ttf")

#define USER_SWITCH_ICONS_PATH BRLS_ASSET("User-Switch-Icons.ttf")

namespace brls
{

void GLFWFontLoader::loadFonts()
{
    // Regular
    // Try to use user-provided font first, fallback to Inter
    if (access(USER_REGULAR_PATH, F_OK) != -1)
        this->loadFontFromFile(FONT_REGULAR, USER_REGULAR_PATH);
    else
        this->loadFontFromFile(FONT_REGULAR, INTER_FONT_PATH);

    // Korean unimplemented

    // Switch icons
    // Only supports user-provided font
    this->loadFontFromFile(FONT_SWITCH_ICONS, USER_SWITCH_ICONS_PATH);

    // Material icons
    this->loadMaterialFromResources();
}

} // namespace brls
