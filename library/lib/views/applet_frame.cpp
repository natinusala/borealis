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

#include <borealis/core/logger.hpp>
#include <borealis/views/applet_frame.hpp>

namespace brls
{

const std::string appletFrameXML = R"xml(
    <brls:Box
        width="auto"
        height="auto"
        axis="column"
        justifyContent="spaceBetween">

        <!-- Header -->
        <brls:Box
            width="auto"
            height="@style/brls/applet_frame/header_height"
            axis="row"
            paddingTop="@style/brls/applet_frame/header_padding_top_bottom"
            paddingBottom="@style/brls/applet_frame/header_padding_top_bottom"
            paddingLeft="@style/brls/applet_frame/header_padding_sides"
            paddingRight="@style/brls/applet_frame/header_padding_sides"
            marginLeft="@style/brls/applet_frame/padding_sides"
            marginRight="@style/brls/applet_frame/padding_sides"
            lineColor="@theme/brls/applet_frame/separator"
            lineBottom="1px">

            <brls:Image
                id="brls/applet_frame/title_icon"
                width="auto"
                height="auto"
                marginRight="@style/brls/applet_frame/header_image_title_spacing"
                visibility="gone" />

            <brls:Label
                id="brls/applet_frame/title_label"
                width="auto"
                height="auto"
                marginTop="@style/brls/applet_frame/header_title_top_offset"
                fontSize="@style/brls/applet_frame/header_title_font_size" />

        </brls:Box>

        <!-- Content will be injected here with grow="1.0" -->

        <!--
            Footer
            Direction inverted so that the bottom left text can be
            set to visibility="gone" without affecting the hint
        -->
        <brls:Box
            width="auto"
            height="@style/brls/applet_frame/footer_height"
            axis="row"
            direction="rightToLeft"
            paddingLeft="@style/brls/applet_frame/footer_padding_sides"
            paddingRight="@style/brls/applet_frame/footer_padding_sides"
            paddingTop="@style/brls/applet_frame/footer_padding_top_bottom"
            paddingBottom="@style/brls/applet_frame/footer_padding_top_bottom"
            marginLeft="@style/brls/applet_frame/padding_sides"
            marginRight="@style/brls/applet_frame/padding_sides"
            lineColor="@theme/brls/applet_frame/separator"
            lineTop="1px"
            justifyContent="spaceBetween" >

            <brls:Rectangle
                width="272px"
                height="auto"
                color="#FF0000" />

            <brls:Rectangle
                width="75px"
                height="auto"
                color="#FF00FF" />

        </brls:Box>

    </brls:Box>
)xml";

AppletFrame::AppletFrame()
{
    this->inflateFromXMLString(appletFrameXML);

    this->title = (Label*)this->getView("brls/applet_frame/title_label");
    this->icon  = (Image*)this->getView("brls/applet_frame/title_icon");

    this->registerStringXMLAttribute("title", [this](std::string value) {
        this->setTitle(value);
    });

    this->registerFilePathXMLAttribute("icon", [this](std::string value) {
        this->setIconFromFile(value);
    });
}

void AppletFrame::setIconFromRes(std::string name)
{
    this->icon->setVisibility(Visibility::VISIBLE);
    this->icon->setImageFromRes(name);
}

void AppletFrame::setIconFromFile(std::string path)
{
    this->icon->setVisibility(Visibility::VISIBLE);
    this->icon->setImageFromFile(path);
}

void AppletFrame::setTitle(std::string title)
{
    this->title->setText(title);
}

void AppletFrame::setContentView(View* view)
{
    if (this->contentView)
    {
        // Remove the node
        this->removeView(this->contentView);
        this->contentView = nullptr;
    }

    if (!view)
        return;

    this->contentView = view;

    this->contentView->setDimensions(View::AUTO, View::AUTO);
    this->contentView->setGrow(1.0f);

    this->addView(this->contentView, 1);
}

void AppletFrame::handleXMLElement(tinyxml2::XMLElement* element)
{
    if (this->contentView)
        throw std::logic_error("brls:AppletFrame can only have one child XML element");

    View* view = View::createFromXMLElement(element);
    this->setContentView(view);
}

View* AppletFrame::create()
{
    return new AppletFrame();
}

} // namespace brls
