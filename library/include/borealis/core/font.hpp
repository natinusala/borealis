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

#pragma once

#include <string>
#include <unordered_map>

namespace brls
{

static constexpr const int FONT_INVALID = -1;

static const std::string FONT_REGULAR        = "regular"; // regular Latin font
static const std::string FONT_KOREAN_REGULAR = "korean"; // regular Korean font
static const std::string FONT_MATERIAL_ICONS = "material"; // Material icons font
static const std::string FONT_SWITCH_ICONS   = "switch"; // Switch icons font (see the HOS shared symbols font for an example)

typedef std::unordered_map<std::string, int> FontStash;

// Platform interface to load fonts from disk or other sources (system / shared font...)
class FontLoader
{
  public:
    virtual ~FontLoader() { }
    /**
     * Called once on init to load every font in the font stash.
     *
     * The implementation must use the Application::loadFont and
     * Application::loadFontFromMemory methods to load as much as possible
     * of the "built-in" fonts defined in the FONT_* constants above.
     */
    virtual void loadFonts() = 0;

  protected:
    /**
     * Convenience method to load a font from a file path
     * with some more logging.
     */
    bool loadFontFromFile(std::string fontName, std::string filePath);

    /**
     * Can be called internally to load the Material icons font from resources.
     * Returns true if the operation succeeds.
     */
    bool loadMaterialFromResources();
};

} // namespace brls
