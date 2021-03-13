/*
    Copyright 2021 XITRIX

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

#include <borealis/core/metrics/point.hpp>
#include <borealis/core/metrics/size.hpp>

namespace brls
{

// A structure that contains the location and dimensions of a rectangle.
struct Rect
{
    Point origin; // A point that specifies the coordinates of the rectangle’s origin.
    Size size; // A size that specifies the height and width of the rectangle.

    // Creates a rectangle with origin (0,0) and size (0,0).
    Rect()
    {
        this->origin = Point();
        this->size   = Size();
    }

    // Creates a rectangle with the specified origin and size.
    Rect(Point origin, Size size)
    {
        this->origin = origin;
        this->size   = size;
    }

    // Creates a rectangle with coordinates and dimensions specified as float values.
    Rect(float x, float y, float width, float height)
    {
        this->origin = Point(x, y);
        this->size   = Size(width, height);
    }

    float getWidth() const { return this->size.width; } // Returns the width of a rectangle.
    float getHeight() const { return this->size.height; } // Returns the height of a rectangle.

    float getMinX() const { return this->origin.x; } // Returns the smallest value for the x-coordinate of the rectangle.
    float getMinY() const { return this->origin.y; } // Returns the smallest value for the y-coordinate of the rectangle.

    float getMidX() const { return this->origin.x + getWidth() / 2; } // Returns the x-coordinate that establishes the center of a rectangle.
    float getMidY() const { return this->origin.y + getHeight() / 2; } // Returns the y-coordinate that establishes the center of the rectangle.

    float getMaxX() const { return this->origin.x + getWidth(); } // Returns the largest value of the x-coordinate for the rectangle.
    float getMaxY() const { return this->origin.y + getHeight(); } // Returns the largest value for the y-coordinate of the rectangle.

    bool operator==(const Rect& other) const
    {
        return origin == other.origin && size == other.size;
    }

    // Returns true if point is inside this Rect
    bool pointInside(Point point)
    {
        return getMinX() <= point.x && getMaxX() >= point.x && getMinY() <= point.y && getMaxY() >= point.y;
    }

    std::string describe()
    {
        return "X: " + std::to_string((int)getMinX()) + ", Y: " + std::to_string((int)getMinY()) + ", W: " + std::to_string((int)getWidth()) + ", H: " + std::to_string((int)getHeight());
    }
};

} // namespace brls
