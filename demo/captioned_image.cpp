/*
    Borealis, a Nintendo Switch UI Library
    Copyright (C) 2020-2021  natinusala

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

#include "captioned_image.hpp"

CaptionedImage::CaptionedImage()
{
    // Load the XML file and inflate ourself with its content
    // The top-level Box in the XML corresponds to us, and every XML child
    // is added to our children (and the attributes are applied)
    // The CaptionedImage instance basically becomes what's written in the XML
    this->inflateFromXMLRes("xml/views/captioned_image.xml");

    // Get the handle to all our children views (that exist because we inflated from the XML)
    this->image = (brls::Image*)this->getView("image");
    this->label = (brls::Label*)this->getView("label");

    // The label stays hidden until focused, so hide it right away
    this->label->hide([] {});

    // Forward Image and Label XML attributes
    this->forwardXMLAttribute("scalingType", this->image);
    this->forwardXMLAttribute("image", this->image);
    this->forwardXMLAttribute("focusUp", this->image);
    this->forwardXMLAttribute("focusRight", this->image);
    this->forwardXMLAttribute("focusDown", this->image);
    this->forwardXMLAttribute("focusLeft", this->image);
    this->forwardXMLAttribute("imageWidth", this->image, "width");
    this->forwardXMLAttribute("imageHeight", this->image, "height");

    this->forwardXMLAttribute("caption", this->label, "text");

    this->addGestureRecognizer(new brls::TapGestureRecognizer([this](){
        brls::Application::giveFocus(this);
    }));
}

void CaptionedImage::onChildFocusGained(brls::View* directChild, brls::View* focusedView)
{
    // Called when a child of ours gets focused, in that case it's the Image

    Box::onChildFocusGained(directChild, focusedView);

    this->label->show([] {});
}

void CaptionedImage::onChildFocusLost(brls::View* directChild, brls::View* focusedView)
{
    // Called when a child of ours losts focused, in that case it's the Image

    Box::onChildFocusLost(directChild, focusedView);

    this->label->hide([] {});
}

brls::View* CaptionedImage::create()
{
    // Called by the XML engine to create a new CaptionedImage
    return new CaptionedImage();
}
