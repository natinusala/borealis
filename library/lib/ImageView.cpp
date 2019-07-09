/*
    Borealis, a Nintendo Switch UI Library
    Copyright (C) 2019  WerWolv

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

#include <ImageView.hpp>
#include <cstring>

ImageView::ImageView(string imagePath, unsigned int width, unsigned int height) : imagePath(imagePath)
{
    this->imageBuffer = nullptr;
    this->textureRedraw = true;
    this->texture = -1;
    
    setOpacity(1.0F);

    setWidth(width);
    setHeight(height);
}

ImageView::ImageView(unsigned char *buffer, size_t bufferSize, unsigned int width, unsigned int height)
{
    this->imageBuffer = new unsigned char[bufferSize];
    std::memcpy(this->imageBuffer, buffer, bufferSize);
    this->imageBufferSize = bufferSize;

    this->textureRedraw = true;
    this->texture = -1;

    setOpacity(1.0F);

    setWidth(width);
    setHeight(height);
}

ImageView::~ImageView() {
    if (this->imageBuffer != nullptr)
        delete[] this->imageBuffer;
}

void ImageView::draw(NVGcontext *vg, int x, int y, unsigned width, unsigned height, Style *style, FrameContext *ctx)
{
    nvgSave(vg);

    if (textureRedraw) {
        if (texture != -1)
            nvgDeleteImage(vg, texture);

        if (!imagePath.empty())
            texture = nvgCreateImage(vg, imagePath.c_str(), 0);
        else if (imageBuffer != nullptr)
            texture = nvgCreateImageMem(vg, 0, this->imageBuffer, this->imageBufferSize);
        
        nvgImageSize(vg, texture, &imageWidth, &imageHeight);

        float viewAspectRatio = static_cast<float>(getWidth()) / static_cast<float>(getHeight());
        float imageAspectRatio = static_cast<float>(imageWidth) / static_cast<float>(imageHeight);

        switch (scaleType) {
            case ScaleType::NO_RESIZE:
                break;
            case ScaleType::FIT:
                if (viewAspectRatio >= imageAspectRatio) {
                    imageHeight = getHeight();
                    imageWidth = imageHeight * imageAspectRatio;
                } else {
                    imageWidth = getWidth();
                    imageHeight = imageWidth * imageAspectRatio;
                }
                break;
            case ScaleType::CROP:
                if (viewAspectRatio < imageAspectRatio) {
                    imageHeight = getHeight();
                    imageWidth = imageHeight * imageAspectRatio;
                } else {
                    imageWidth = getWidth();
                    imageHeight = imageWidth * imageAspectRatio;
                }
                break;
            case ScaleType::SCALE:
                imageWidth = getWidth();
                imageHeight = getHeight();
                break;
        }

        imgPaint = nvgImagePattern(vg, x, y, imageWidth, imageHeight, 0, texture, opacity);

        textureRedraw = false;
    }
    
    nvgBeginPath(vg);
    nvgRect(vg, x, y, imageWidth, imageHeight);
    nvgFillPaint(vg, imgPaint);
    nvgFill(vg);

    nvgRestore(vg);
}

void ImageView::layout(NVGcontext* vg, Style *style, FontStash *stash)
{
    this->imageX = getX();
    this->imageY = getY();
}

void ImageView::setImage(unsigned char *buffer, size_t bufferSize)
{
    if (this->imageBuffer != nullptr)
        delete[] this->imageBuffer;
    
    this->imageBuffer = new unsigned char[bufferSize];

    std::memcpy(this->imageBuffer, buffer, bufferSize);
    this->imageBufferSize = bufferSize;

    this->imagePath = "";

    textureRedraw = true;
}

void ImageView::setImage(string imagePath)
{
    this->imagePath = imagePath;

    if (this->imageBuffer != nullptr)
        delete[] this->imageBuffer;

    this->imageBuffer = nullptr;
    this->textureRedraw = true;
    this->texture = -1;
}

void ImageView::setOpacity(float opacity)
{
    this->opacity = opacity;
    textureRedraw = true;
}

void ImageView::setScaleType(ScaleType scaleType) {
    this->scaleType = scaleType;
    textureRedraw = true;
}
