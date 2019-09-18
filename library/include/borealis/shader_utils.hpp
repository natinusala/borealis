/*
    Borealis, a Nintendo Switch UI Library
    Copyright (C) 2019  WerWolv
    Copyright (C) 2019  natinusala

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

#include <glad.h>

namespace brls
{

    class ShaderUtils
    {
    public:
        static GLuint createAndCompileShader(GLenum type, const char* source);

        struct Vertex
        {
            float position[3];
            float color[3];
        };

        static inline const Vertex rectVertices[] =
        {
            { { -1.0f, -1.0f, 0.0f }, { 0.0f, 0.0f, 0.0f } },
            { { 3.0f, -1.0f, 0.0f }, { 2.0f, 0.0f, 0.0f } },
            { { -1.0f, 3.0f, 0.0f }, { 0.0f, 2.0f, 1.0f } },
        };
    };

}