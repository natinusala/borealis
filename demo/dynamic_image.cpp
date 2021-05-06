/*
    Copyright 2019 WerWolv
    Copyright 2019 p-sam
    Copyright 2020-2021 natinusala

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

#include "dynamic_image.hpp"

DynamicImage::DynamicImage()
{
    // Load the XML file and inflate ourself with its content
    this->inflateFromXMLRes("xml/views/dynamic_image.xml");

    // Forward Image XML attributes
    this->forwardXMLAttribute("scalingType", this->image);
    this->forwardXMLAttribute("image", this->image);
    this->forwardXMLAttribute("focusUp", this->image);
    this->forwardXMLAttribute("focusRight", this->image);
    this->forwardXMLAttribute("focusDown", this->image);
    this->forwardXMLAttribute("focusLeft", this->image);
    this->forwardXMLAttribute("imageWidth", this->image, "width");
    this->forwardXMLAttribute("imageHeight", this->image, "height");

    // Register a click action for this button
    BRLS_REGISTER_CLICK_BY_ID("image", this->onImageClicked);
}

bool DynamicImage::onImageClicked(brls::View* view)
{
    // Retrieve the image size
    int height = this->image->getHeight();
    int width = this->image->getWidth();

    // Generate the pixel data for a new image
    size_t bufferSize = height * width * 4; // 4 bytes per pixel (RGBA)
    unsigned char* imageData = (unsigned char*)calloc(1, bufferSize);

    for (int y = 0; y < height; y++)
    {
        for (int x = 0; x < width; x++)
        {
            memset(&imageData[(y * width + x) * 4 + 3], (unsigned char)255, sizeof(unsigned char));                         // Alpha
            memset(&imageData[(y * width + x) * 4 + 2], (unsigned char)((y+x)*255/(height+width)), sizeof(unsigned char));  // Blue
            memset(&imageData[(y * width + x) * 4 + 1], (unsigned char)(x*255/width), sizeof(unsigned char));               // Green
            memset(&imageData[(y * width + x) * 4 + 0], (unsigned char)(y*255/height), sizeof(unsigned char));              // Red
        }
    }

    // Display the image
    this->image->setImageFromMemory(imageData, width, height);

    // Free the image buffer
    free(imageData);

    return true;
}

brls::View* DynamicImage::create()
{
    // Called by the XML engine to create a new DynamicImage
    return new DynamicImage();
}
