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

#include <string>

namespace brls
{

// A structure that contains a point in a two-dimensional coordinate system.
struct Point
{
    float x; // The x-coordinate of the point.
    float y; // The y-coordinate of the point.

    // Creates a point with location (0,0).
    Point();

    // Creates a point with coordinates specified as float values.
    Point(float x, float y);

    Point operator+(const Point& a) const;
    Point operator-(const Point& a) const;
    Point operator/(const float& a) const;
    Point operator*(const float& a) const;
    bool operator==(const Point& other) const;
};

// A structure that contains width and height values.
struct Size
{
    float width; // A width value.
    float height; // A height value.

    // Creates a size with zero width and height.
    Size();

    // Creates a size with dimensions specified as float values.
    Size(float width, float height);

    Size operator+(const Size& a) const;
    Size operator-(const Size& a) const;
    Size operator/(const float& a) const;
    Size operator*(const float& a) const;
    bool operator==(const Size& other) const;
};

// Rect
// A structure that contains the location and dimensions of a rectangle.
struct Rect
{
    Point origin; // A point that specifies the coordinates of the rectangle’s origin.
    Size size; // A size that specifies the height and width of the rectangle.

    // Creates a rectangle with origin (0,0) and size (0,0).
    Rect();

    // Creates a rectangle with the specified origin and size.
    Rect(Point origin, Size size);

    // Creates a rectangle with coordinates and dimensions specified as float values.
    Rect(float x, float y, float width, float height);

    // Returns the width of a rectangle.
    float getWidth() const;
    // Returns the height of a rectangle.
    float getHeight() const;

    // Returns the smallest value for the x-coordinate of the rectangle.
    float getMinX() const;
    // Returns the smallest value for the y-coordinate of the rectangle.
    float getMinY() const;

    // Returns the x-coordinate that establishes the center of a rectangle.
    float getMidX() const;
    // Returns the y-coordinate that establishes the center of the rectangle.
    float getMidY() const;

    // Returns the largest value of the x-coordinate for the rectangle.
    float getMaxX() const;
    // Returns the largest value for the y-coordinate of the rectangle.
    float getMaxY() const;

    bool operator==(const Rect& other) const;

    // Returns true if point is inside this Rect.
    bool pointInside(Point point);

    // Returns string with description of current Rect.
    std::string describe();

    // Returns true if two rects have shared area.
    bool collideWith(const Rect& other) const;

    // Returns Rect with offset by presented Point.
    Rect offsetBy(const Point& origin) const;
};

} // namespace brls
