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
            textAlign="right" />

    </brls:Box>
)xml";

Header::Header()
{
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
