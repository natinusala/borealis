/*
    Copyright 2021 Jonathan Verbeek

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

#include "random_rgba_image.hpp"

RandomRGBAImage::RandomRGBAImage()
{
    // Load the XML file and inflate ourself with its content
    this->inflateFromXMLRes("xml/views/random_rgba_image.xml");

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

bool RandomRGBAImage::onImageClicked(brls::View* view)
{
    // Generate a new image
    this->generateRandomImage();

    return true;
}

unsigned char interp(unsigned char src, unsigned char dst, float alpha)
{
    return src * (1.0 - alpha) + dst * alpha;
}

void RandomRGBAImage::generateRandomImage()
{
    // Retrieve the image size
    int height = this->image->getHeight();
    int width = this->image->getWidth();

    // Allocate the RGBA image buffer
    size_t bufferSize = height * width * 4; // 4 bytes per pixel (RGBA8888)
    unsigned char* imageData = (unsigned char*)calloc(1, bufferSize);

    // Randomly generate two colors for a gradient
    unsigned char color1[3] = { std::rand() % 255, std::rand() % 255, std::rand() % 255};
    unsigned char color2[3] = { std::rand() % 255, std::rand() % 255, std::rand() % 255};

    for (int y = 0; y < height; y++)
    {
        for (int x = 0; x < width; x++)
        {
            // Set the pixel data
            memset(&imageData[(y * width + x) * 4 + 3], (unsigned char)255, sizeof(unsigned char)); // Alpha
            memset(&imageData[(y * width + x) * 4 + 2], (unsigned char)interp(color1[2], color2[2], (float)x/(float)width), sizeof(unsigned char)); // Blue
            memset(&imageData[(y * width + x) * 4 + 1], (unsigned char)interp(color1[1], color2[1], (float)x/(float)width), sizeof(unsigned char)); // Green
            memset(&imageData[(y * width + x) * 4 + 0], (unsigned char)interp(color1[0], color2[0], (float)x/(float)width), sizeof(unsigned char)); // Red
        }
    }

    // Display the image
    this->image->setImageFromRGBA(imageData, width, height);

    // Free the image buffer
    free(imageData);
}

brls::View* RandomRGBAImage::create()
{
    // Called by the XML engine to create a new DynamicImage
    return new RandomRGBAImage();
}
