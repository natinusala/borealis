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

#pragma once

#include <borealis/core/view.hpp>

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
};

// This dictates what interpolation to use when down / up scaling the image
enum class ImageInterpolation
{
    LINEAR,
    NEAREST
};

// Alignment of the image inside the view for FIT and CROP scaling types
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
     * Sets the interpolation method for the image. Default is LINEAR.
     *
     * As the interpolation is set when the texture is created, and since we don't
     * want to always store the image buffer in the view, this only takes effect
     * after (re) loading the image using the setImage* methods.
     *
     * If you are using the interpolation XML attribute, you have to set it before the
     * actual image attribute.
     */
    void setInterpolation(ImageInterpolation interpolation);

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
    ImageScalingType scalingType     = ImageScalingType::FIT;
    ImageAlignment align             = ImageAlignment::CENTER;
    ImageInterpolation interpolation = ImageInterpolation::LINEAR;

    int texture = 0;

    NVGpaint paint;

    void invalidateImageBounds();
    int getImageFlags();

    float originalImageWidth  = 0;
    float originalImageHeight = 0;

    float imageX      = 0;
    float imageY      = 0;
    float imageHeight = 0;
    float imageWidth  = 0;
};

} // namespace brls
