/*
    Borealis, a Nintendo Switch UI Library
    Copyright (C) 2021  natinusala

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <https://www.gnu.org/licenses/>.
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
