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
#include <Application.hpp>

ImageView::ImageView(string imagePath, unsigned int width, unsigned int height)
{   
    setImage(imagePath);    
    setOpacity(1.0F);
}

ImageView::ImageView(unsigned char *buffer, size_t bufferSize, unsigned int width, unsigned int height)
{
    setImage(buffer, bufferSize);
    setOpacity(1.0F);
}

ImageView::~ImageView() {
    if (this->imageBuffer != nullptr)
        delete[] this->imageBuffer;

    if (texture != -1)
        nvgDeleteImage(Application::getNVGContext(), this->texture);
}

void ImageView::draw(NVGcontext *vg, int x, int y, unsigned width, unsigned height, Style *style, FrameContext *ctx)
{
    nvgSave(vg);
    
    if (this->texture != -1) {
        nvgBeginPath(vg);
        nvgRect(vg, x + this->imageX, y + this->imageY, this->imageWidth, this->imageHeight);
        nvgFillPaint(vg, this->imgPaint);
        nvgFill(vg);
    }

    nvgRestore(vg);
}

void ImageView::layout(NVGcontext* vg, Style *style, FontStash *stash)
{
    static int origViewWidth = getWidth();
    static int origViewHeight = getHeight();

    if (this->texture != -1)
        nvgDeleteImage(vg, this->texture);

    if (!this->imagePath.empty())
        this->texture = nvgCreateImage(vg, this->imagePath.c_str(), 0);
    else if (this->imageBuffer != nullptr)
        this->texture = nvgCreateImageMem(vg, 0, this->imageBuffer, this->imageBufferSize);
    
    nvgImageSize(vg, this->texture, &this->imageWidth, &this->imageHeight);

    setWidth(origViewWidth);
    setHeight(origViewHeight);

    this->imageX = 0;
    this->imageY = 0;

    float viewAspectRatio = static_cast<float>(getWidth()) / static_cast<float>(getHeight());
    float imageAspectRatio = static_cast<float>(this->imageWidth) / static_cast<float>(this->imageHeight);

    switch (imageScaleType) {
        case ImageScaleType::NO_RESIZE:
            this->imageX = (origViewWidth - this->imageWidth) / 2.0F;
            this->imageY = (origViewHeight - this->imageHeight) / 2.0F;
            break;
        case ImageScaleType::FIT:
            if (viewAspectRatio >= imageAspectRatio) {
                this->imageHeight = getHeight();
                this->imageWidth = this->imageHeight * imageAspectRatio;
                this->imageX = (origViewWidth - this->imageWidth) / 2.0F;
            } else {
                this->imageWidth = getWidth();
                this->imageHeight = this->imageWidth * imageAspectRatio;
                this->imageY = (origViewHeight - this->imageHeight) / 2.0F;
            }
            break;
        case ImageScaleType::CROP:
            if (viewAspectRatio < imageAspectRatio) {
                this->imageHeight = getHeight();
                this->imageWidth = this->imageHeight * imageAspectRatio;
                this->imageX = (origViewWidth - this->imageWidth) / 2.0F;
            } else {
                this->imageWidth = getWidth();
                this->imageHeight = this->imageWidth * imageAspectRatio;
                this->imageY = (origViewHeight - this->imageHeight) / 2.0F;
            }
            break;
        case ImageScaleType::SCALE:
            this->imageWidth = getWidth();
            this->imageHeight = getHeight();
            break;
        case ImageScaleType::VIEW_RESIZE:
            setWidth(this->imageWidth);
            setHeight(this->imageHeight);
            break;
    }

    this->imgPaint = nvgImagePattern(vg, getX() + this->imageX, getY() + this->imageY, this->imageWidth, this->imageHeight, 0, this->texture, this->opacity);
}

void ImageView::setImage(unsigned char *buffer, size_t bufferSize)
{
    if (this->imageBuffer != nullptr)
        delete[] this->imageBuffer;
    
    this->imagePath = "";

    this->imageBuffer = new unsigned char[bufferSize];
    std::memcpy(this->imageBuffer, buffer, bufferSize);
    this->imageBufferSize = bufferSize;

    this->texture = -1;
    invalidate();
}

void ImageView::setImage(string imagePath)
{
    this->imagePath = imagePath;

    if (this->imageBuffer != nullptr)
        delete[] this->imageBuffer;

    this->imageBuffer = nullptr;
    this->texture = -1;
    invalidate();
}

void ImageView::setOpacity(float opacity)
{
    this->opacity = opacity;
    invalidate();
}

void ImageView::setImageScaleType(ImageScaleType imageScaleType) {
    this->imageScaleType = imageScaleType;
    invalidate();
}
