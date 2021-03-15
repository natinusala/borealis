/*
    Copyright 2021 natinusala

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

#include <borealis/core/geometry.hpp>

namespace brls
{

// Point
Point::Point()
    : Point(0.0f, 0.0f)
{
}

Point::Point(float x, float y)
{
    this->x = x;
    this->y = y;
}

Point Point::operator+(const Point& a) const
{
    return Point(a.x + x, a.y + y);
}

Point Point::operator-(const Point& a) const
{
    return Point(a.x - x, a.y - y);
}

Point Point::operator/(const float& a) const
{
    return Point(x / a, y / a);
}

Point Point::operator*(const float& a) const
{
    return Point(x * a, y * a);
}

bool Point::operator==(const Point& other) const
{
    return x == other.x && y == other.y;
}

// Size
Size::Size()
    : Size(0.0f, 0.0f)
{
}

Size::Size(float width, float height)
{
    this->width  = width;
    this->height = height;
}

Size Size::operator+(const Size& a) const
{
    return Size(a.width + width, a.height + height);
}

Size Size::operator-(const Size& a) const
{
    return Size(a.width - width, a.height - height);
}

Size Size::operator/(const float& a) const
{
    return Size(width / a, height / a);
}

Size Size::operator*(const float& a) const
{
    return Size(width * a, height * a);
}

bool Size::operator==(const Size& other) const
{
    return width == other.width && height == other.height;
}

// Rect
Rect::Rect()
{
    this->origin = Point();
    this->size   = Size();
}

Rect::Rect(Point origin, Size size)
{
    this->origin = origin;
    this->size   = size;
}

Rect::Rect(float x, float y, float width, float height)
{
    this->origin = Point(x, y);
    this->size   = Size(width, height);
}

float Rect::getWidth() const { return this->size.width; }
float Rect::getHeight() const { return this->size.height; }

float Rect::getMinX() const { return this->origin.x; }
float Rect::getMinY() const { return this->origin.y; }

float Rect::getMidX() const { return this->origin.x + getWidth() / 2; }
float Rect::getMidY() const { return this->origin.y + getHeight() / 2; }

float Rect::getMaxX() const { return this->origin.x + getWidth(); }
float Rect::getMaxY() const { return this->origin.y + getHeight(); }

bool Rect::operator==(const Rect& other) const
{
    return origin == other.origin && size == other.size;
}

bool Rect::pointInside(Point point)
{
    return getMinX() <= point.x && getMaxX() >= point.x && getMinY() <= point.y && getMaxY() >= point.y;
}

std::string Rect::describe()
{
    return "X: " + std::to_string((int)getMinX()) + ", Y: " + std::to_string((int)getMinY()) + ", W: " + std::to_string((int)getWidth()) + ", H: " + std::to_string((int)getHeight());
}

} // namespace brls
