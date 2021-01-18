/*
    Borealis, a Nintendo Switch UI Library
    Copyright (C) 2019  WerWolv
    Copyright (C) 2019  p-sam
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

#pragma once

#include <borealis/views/view.hpp>

namespace brls
{

// This dictates what to do with the image if there is not
// enough room for the view to grow and display the whole image,
// or if the view is bigger than the image
enum class ImageScalingType
{
    // The image is scaled to fit the view boundaries, aspect ratio is conserved
    FIT,
    // The image is stretched to fit the view boundaries (aspect ratio is not conserved). The original image dimensions are entirely ignored in the layout process.
    STRETCH,
    // The image is either cropped (not enough space) or untouched (too much space)
    CROP,

    // TODO: try to do a TILED type with NVGimageFlags
};

// Alignment of the image inside the view for FIT and CROP scaling types
// TODO: add diagonals too
enum class ImageAlignment
{
    TOP,
    RIGHT,
    BOTTOM,
    LEFT,
    CENTER,
};

// An image. The view will try to grow as much
// as possible to fit the image. The scaling type dictates
// what to do with the image if there is not enough or too much space
// for the view compared to the image inside.
// Supported formats are: JPG, PNG, TGA, BMP and GIF (not animated).
class Image : public View
{
  public:
    Image();
    ~Image();

    void draw(NVGcontext* vg, float x, float y, float width, float height, Style style, FrameContext* ctx) override;
    void onLayout() override;

    /**
     * Sets the image from the given resource name.
     *
     * See Image class documentation for the list of supported
     * image formats.
     */
    void setImageFromRes(std::string name);

    /**
     * Sets the image from the given file path.
     *
     * See Image class documentation for the list of supported
     * image formats.
     */
    void setImageFromFile(std::string path);

    /**
     * Sets the scaling type for this image.
     *
     * Default is FIT.
     */
    void setScalingType(ImageScalingType scalingType);

    ImageScalingType getScalingType();

    /**
     * Sets the alignment of the image inside the view.
     * Only used if scaling it set to FIT when the aspect ratio is different
     * or CROP, to change the region of the image to draw.
     *
     * Default is CENTER.
     */
    void setImageAlign(ImageAlignment align);

    int getTexture();
    float getOriginalImageWidth();
    float getOriginalImageHeight();

    static View* create();

  private:
    ImageScalingType scalingType = ImageScalingType::FIT;
    ImageAlignment align         = ImageAlignment::CENTER;

    int texture = 0;

    NVGpaint paint;

    void invalidateImageBounds();

    float originalImageWidth  = 0;
    float originalImageHeight = 0;

    float imageX      = 0;
    float imageY      = 0;
    float imageHeight = 0;
    float imageWidth  = 0;
};

} // namespace brls
