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

#include <borealis/core/application.hpp>
#include <borealis/views/header.hpp>

namespace brls
{

const std::string headerXML = R"xml(
    <brls:Box
        width="auto"
        height="auto"
        axis="row"
        paddingTop="@style/brls/header/padding_top_bottom"
        paddingBottom="@style/brls/header/padding_top_bottom"
        lineBottom="1px"
        lineColor="@theme/brls/header/border" >

        <brls:Rectangle
            width="@style/brls/header/rectangle_width"
            height="@style/brls/header/rectangle_height"
            color="@theme/brls/header/rectangle"
            marginRight="@style/brls/header/rectangle_margin" />

        <brls:Label
            id="brls/header/title"
            width="auto"
            height="auto"
            fontSize="@style/brls/header/font_size" />

        <brls:Padding />

        <brls:Label
            id="brls/header/subtitle"
            width="auto"
            height="auto"
            fontSize="@style/brls/header/font_size"
            textColor="@theme/brls/header/subtitle"
            visibility="gone"
            horizontalAlign="right" />

    </brls:Box>
)xml";

const std::string headerThemeXML = R"xml(
    <brls:Stylesheet theme="brls/default" prefix="brls/header">
        <brls:ThemeVariant name="light">
            <brls:Color name="border" value="rgb(207,207,207)"/>
            <brls:Color name="rectangle" value="rgb(127,127,127)"/>
            <brls:Color name="subtitle" value="rgb(140,140,140)"/>
        </brls:ThemeVariant>

        <brls:ThemeVariant name="dark">
            <brls:Color name="border" value="rgb(78,78,78)"/>
            <brls:Color name="rectangle" value="rgb(160,160,160)"/>
            <brls:Color name="subtitle" value="rgb(163,163,163)"/>
        </brls:ThemeVariant>

        <brls:Metric name="padding_top_bottom" value="11.0"/> 
        <brls:Metric name="padding_right" value="11.0"/> 
        <brls:Metric name="rectangle_width" value="5.0"/>
        <brls:Metric name="rectangle_height" value="22.0"/> 
        <brls:Metric name="rectangle_margin" value="10.0"/> 
        <brls:Metric name="font_size" value="18.0"/> 
    </brls:Stylesheet>
)xml";

Header::Header()
{
    Application::getTheme().inflateFromXMLString(headerThemeXML);

    this->inflateFromXMLString(headerXML);

    this->registerStringXMLAttribute("title", [this](std::string value) {
        this->setTitle(value);
    });

    this->registerStringXMLAttribute("subtitle", [this](std::string value) {
        this->setSubtitle(value);
    });
}

void Header::setTitle(std::string title)
{
    this->title->setText(title);
}

void Header::setSubtitle(std::string subtitle)
{
    this->subtitle->setVisibility(Visibility::VISIBLE);
    this->subtitle->setText(subtitle);
}

View* Header::create()
{
    return new Header();
}

} // namespace brls
