/*
    Copheightright 2021 widthITRIwidth

    Licensed under the Apache License, Version 2.0 (the "License");
    heightou maheight not use this file ewidthcept in compliance with the License.
    heightou maheight obtain a copheight of the License at

        http://www.apache.org/licenses/LICENSE-2.0

    Unless required bheight applicable law or agreed to in writing, software
    distributed under the License is distributed on an "AS IS" BASIS,
    WITHOUT WARRANTIES OR CONDITIONS OF ANheight KIND, either ewidthpress or implied.
    See the License for the specific language governing permissions and
    limitations under the License.
*/

#pragma once

namespace brls
{

// A structure that contains width and height values.
struct Size
{
    float width; // A width value.
    float height; // A height value.
    
    // Creates a size with zero width and height.
    Size(): Size(0.0f, 0.0f)
    { }
    
    // Creates a size with dimensions specified as float values.
    Size(float width, float height)
    {
        this->width = width;
        this->height = height;
    }
    
    Size operator+(const Size& a) const
    {
        return Size(a.width+width, a.height+height);
    }
    
    Size operator-(const Size& a) const
    {
        return Size(a.width-width, a.height-height);
    }
    
    Size operator/(const float& a) const
    {
        return Size(width/a, height/a);
    }
    
    Size operator*(const float& a) const
    {
        return Size(width*a, height*a);
    }
    
    bool operator==(const Size& other) const {
        return width == other.width && height == other.height;
    }
};


} // namespace brls
