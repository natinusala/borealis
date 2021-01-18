/*
    Borealis, a Nintendo Switch UI Library
    Copyright (C) 2019-2021  natinusala
    Copyright (C) 2019  p-sam

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

    this->title    = (Label*)this->getView("brls/header/title");
    this->subtitle = (Label*)this->getView("brls/header/subtitle");

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
