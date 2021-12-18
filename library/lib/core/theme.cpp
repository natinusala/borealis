/*
    Copyright 2019 natinusala
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

#include <borealis/core/theme.hpp>
#include <borealis/core/logger.hpp>
#include <stdexcept>
#include <algorithm>
#include <cctype>

namespace brls
{
/*
static ThemeValues lightThemeValues = {
    // Generic values
    { "brls/background", nvgRGB(235, 235, 235) },
    { "brls/text", nvgRGB(45, 45, 45) },
    { "brls/backdrop", nvgRGBA(0, 0, 0, 178) },
    { "brls/click_pulse", nvgRGBA(13, 182, 213, 38) }, // same as highlight color1 with different opacity

    // Highlight
    { "brls/highlight/background", nvgRGB(252, 255, 248) },
    { "brls/highlight/color1", nvgRGB(13, 182, 213) },
    { "brls/highlight/color2", nvgRGB(80, 239, 217) },

    // AppletFrame
    { "brls/applet_frame/separator", nvgRGB(45, 45, 45) },

    // Sidebar
    { "brls/sidebar/background", nvgRGB(240, 240, 240) },
    { "brls/sidebar/active_item", nvgRGB(49, 79, 235) },
    { "brls/sidebar/separator", nvgRGB(208, 208, 208) },

    // Header
    { "brls/header/border", nvgRGB(207, 207, 207) },
    { "brls/header/rectangle", nvgRGB(127, 127, 127) },
    { "brls/header/subtitle", nvgRGB(140, 140, 140) },

    // Button
    { "brls/button/primary_enabled_background", nvgRGB(50, 79, 241) },
    { "brls/button/primary_disabled_background", nvgRGB(201, 201, 209) },
    { "brls/button/primary_enabled_text", nvgRGB(255, 255, 255) },
    { "brls/button/primary_disabled_text", nvgRGB(220, 220, 228) },

    { "brls/button/default_enabled_background", nvgRGB(255, 255, 255) },
    { "brls/button/default_disabled_background", nvgRGB(255, 255, 255) },
    { "brls/button/default_enabled_text", nvgRGB(45, 45, 45) },
    { "brls/button/default_disabled_text", nvgRGB(45, 45, 45) },

    { "brls/button/highlight_enabled_text", nvgRGB(49, 79, 235) },
    { "brls/button/highlight_disabled_text", nvgRGB(49, 79, 235) },

    { "brls/button/enabled_border_color", nvgRGB(45, 45, 45) },
    { "brls/button/disabled_border_color", nvgRGB(45, 45, 45) },
};

static ThemeValues darkThemeValues = {
    // Generic values
    { "brls/background", nvgRGB(45, 45, 45) },
    { "brls/text", nvgRGB(255, 255, 255) },
    { "brls/backdrop", nvgRGBA(0, 0, 0, 178) },
    { "brls/click_pulse", nvgRGBA(25, 138, 198, 38) }, // same as highlight color1 with different opacity

    // Highlight
    { "brls/highlight/background", nvgRGB(31, 34, 39) },
    { "brls/highlight/color1", nvgRGB(25, 138, 198) },
    { "brls/highlight/color2", nvgRGB(137, 241, 242) },

    // AppletFrame
    { "brls/applet_frame/separator", nvgRGB(255, 255, 255) },

    // Sidebar
    { "brls/sidebar/background", nvgRGB(50, 50, 50) },
    { "brls/sidebar/active_item", nvgRGB(0, 255, 204) },
    { "brls/sidebar/separator", nvgRGB(81, 81, 81) },

    // Header
    { "brls/header/border", nvgRGB(78, 78, 78) },
    { "brls/header/rectangle", nvgRGB(160, 160, 160) },
    { "brls/header/subtitle", nvgRGB(163, 163, 163) },

    // Button
    { "brls/button/primary_enabled_background", nvgRGB(1, 255, 201) },
    { "brls/button/primary_disabled_background", nvgRGB(83, 87, 86) },
    { "brls/button/primary_enabled_text", nvgRGB(52, 41, 55) },
    { "brls/button/primary_disabled_text", nvgRGB(71, 75, 74) },

    { "brls/button/default_enabled_background", nvgRGB(80, 80, 80) },
    { "brls/button/default_disabled_background", nvgRGB(80, 80, 80) },
    { "brls/button/default_enabled_text", nvgRGB(255, 255, 255) },
    { "brls/button/default_disabled_text", nvgRGB(255, 255, 255) },

    { "brls/button/highlight_enabled_text", nvgRGB(7, 247, 198) },
    { "brls/button/highlight_disabled_text", nvgRGB(7, 247, 198) },

    { "brls/button/enabled_border_color", nvgRGB(255, 255, 255) },
    { "brls/button/disabled_border_color", nvgRGB(255, 255, 255) },
};
*/

Theme::Theme(std::string name) : name{name}
{}

bool validThemeVariant(const std::string themeVariant)
{
    std::string themeVariantLowercase = themeVariant;
    std::transform(themeVariantLowercase.begin(), themeVariantLowercase.end(), 
    themeVariantLowercase.begin(), [](unsigned char c){ return std::tolower(c); });

    if (themeVariantLowercase == "dark" || themeVariantLowercase == "light")
        return true;
    return false;
}

bool startsWith(const std::string& data, const std::string& prefix)
{
    return data.rfind(prefix, 0) == 0;
}

NVGcolor processColorValue(const std::string val)
{
    if (startsWith(val, "#"))
    {
        if (val.size() == 6 + 1)
        {
            unsigned char r, g, b;
            int result = sscanf(val.c_str(), "#%02hhx%02hhx%02hhx", &r, &g, &b);

            if (result != 3)
            {
                Logger::error("Theme: failed to extract hexadecimal color value");
                return nvgRGB(0, 0, 0);
            }

            return nvgRGB(r, g, b);
        }
        else if (val.size() == 8 + 1)
        {
            unsigned char r, g, b, a;
            int result = sscanf(val.c_str(), "#%02hhx%02hhx%02hhx%02hhx", &r, &g, &b, &a);

            if (result != 4)
            {
                Logger::error("Theme: failed to extract hexadecimal color value");
                return nvgRGBA(0, 0, 0, 0);
            }

            return nvgRGBA(r, g, b, a);
        }
    }
    else
    {
        Logger::error("Theme: invalid color value");
        return nvgRGBA(0, 0, 0, 0);
    }
}

float processMetricValue(const std::string val)
{
    try
    {
        return std::stof(val);
    }
    catch (const std::invalid_argument& e)
    {
        Logger::error("Theme: invalid metric value");
        return 0.0f;
    }
    
}

void processValues(tinyxml2::XMLElement *currElem,
                    const std::string prefix,
                    const std::string themeVar,
                    std::unordered_map<std::string, NVGcolor> &colors,
                    std::unordered_map<std::string, float> &metrics)
{
    for (tinyxml2::XMLElement *e = currElem->FirstChildElement(); e != NULL; e = e->NextSiblingElement())
    {
        std::string elemName = std::string(e->Name());
        std::string name = e->Attribute("name");
        std::string value = e->Attribute("value");

        if (elemName == "brls:Color")
            colors[themeVar + "/" + name] = processColorValue(value);
        else if (elemName == "brls:Metric")
            metrics[themeVar + "/" + name] = processMetricValue(value);
        else
            continue;
    }
}

void Theme::inflateFromXMLElement(tinyxml2::XMLElement *element)
{
    if (element == nullptr)
    {
        Logger::error("Theme: NULL root element. This means the XML file is blank (hasn't been written to yet), or the XML file is broken.");
        return;
    }

    if (std::strcmp(element->Attribute("theme"), name.c_str()) != 0)
    {
        Logger::error("Theme: read stylesheet that has different theme name than current theme");
        return;
    }

    std::string prefix = std::string(element->Attribute("prefix"));

    //tinyxml2::XMLElement *element = nullptr;
    for (tinyxml2::XMLElement *e = element->FirstChildElement("brls:ThemeVariant"); e != NULL; e = e->NextSiblingElement("brls:ThemeVariant"))
    {
        std::string currThemeVariant = e->Attribute("name");
        if (!validThemeVariant(currThemeVariant))
            continue;

        std::transform(currThemeVariant.begin(), currThemeVariant.end(), 
        currThemeVariant.begin(), [](unsigned char c){ return std::tolower(c); });

        processValues(e, prefix, currThemeVariant, colors, metrics);
    }
}

void Theme::inflateFromXMLString(const std::string xml)
{
    tinyxml2::XMLDocument doc;
    tinyxml2::XMLError errorCode = doc.Parse(xml.c_str());

    if (errorCode == tinyxml2::XML_SUCCESS)
        return inflateFromXMLElement(doc.RootElement());
    
    Logger::error("TinyXML2 could not parse the XML string. Error code {}.", std::to_string(errorCode));
    Logger::error("More details: {}", doc.ErrorStr());
}

void Theme::inflateFromXMLFile(const std::string path)
{
    tinyxml2::XMLDocument doc;
    tinyxml2::XMLError errorCode = doc.LoadFile(path.c_str());

    if (errorCode == tinyxml2::XML_SUCCESS)
        return inflateFromXMLElement(doc.RootElement());
    
    Logger::error("TinyXML2 could not open the file. Error code {}.", std::to_string(errorCode));
    Logger::error("More details: {}", doc.ErrorStr());
}

float Theme::getMetric(const std::string path, ThemeVariant variant)
{
    std::string themeVar = "light";
    if (variant == ThemeVariant::DARK)
        themeVar = "dark";

    return metrics[themeVar + "/" + path];
}

NVGcolor Theme::getColor(const std::string path, ThemeVariant variant)
{
    std::string themeVar = "light";
    if (variant == ThemeVariant::DARK)
        themeVar = "dark";

    return colors[themeVar + "/" + path];
}

} // namespace brls
