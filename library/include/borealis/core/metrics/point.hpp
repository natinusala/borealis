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

namespace brls
{

// A structure that contains a point in a two-dimensional coordinate system.
struct Point
{
    float x; // The x-coordinate of the point.
    float y; // The y-coordinate of the point.

    // Creates a point with location (0,0).
    Point()
        : Point(0.0f, 0.0f)
    {
    }

    // Creates a point with coordinates specified as float values.
    Point(float x, float y)
    {
        this->x = x;
        this->y = y;
    }

    Point operator+(const Point& a) const
    {
        return Point(a.x + x, a.y + y);
    }

    Point operator-(const Point& a) const
    {
        return Point(a.x - x, a.y - y);
    }

    Point operator/(const float& a) const
    {
        return Point(x / a, y / a);
    }

    Point operator*(const float& a) const
    {
        return Point(x * a, y * a);
    }

    bool operator==(const Point& other) const
    {
        return x == other.x && y == other.y;
    }
};

} // namespace brls
