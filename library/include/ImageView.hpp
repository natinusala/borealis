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

#pragma once

#include <View.hpp>
#include <FrameContext.hpp>

enum class ImageScaleType {
    NO_RESIZE = 0,
    FIT,
    CROP,
    SCALE,
    VIEW_RESIZE
};

// An image
class ImageView : public View
{
    protected:
        void layout(NVGcontext* vg, Style *style, FontStash *stash) override;

    public:
        ImageView(string imagePath, unsigned int width, unsigned int height);
        ImageView(unsigned char *buffer, size_t bufferSize, unsigned int width, unsigned int height);
        ~ImageView();

        void draw(NVGcontext *vg, int x, int y, unsigned width, unsigned height, Style *style, FrameContext *ctx) override;

        void setImage(unsigned char *buffer, size_t bufferSize);
        void setImage(string imagePath);

        void setOpacity(float opacity);
        void setImageScaleType(ImageScaleType imageScaleType);

    private:
        string imagePath;
        unsigned char *imageBuffer;
        size_t imageBufferSize;

        int texture;
        NVGpaint imgPaint;

        ImageScaleType imageScaleType = ImageScaleType::NO_RESIZE;

        int imageX, imageY;
        int imageWidth, imageHeight;
        float opacity;
};
