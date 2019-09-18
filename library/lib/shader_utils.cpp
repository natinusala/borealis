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

#include <borealis/shader_utils.hpp>
#include <borealis/logger.hpp>

namespace brls
{

    GLuint ShaderUtils::createAndCompileShader(GLenum type, const char* source)
    {
        GLint success;
        GLchar msg[512];

        GLuint handle = glCreateShader(type);
        if (!handle)
        {
            Logger::error("%u: cannot create shader", type);
            return 0;
        }
        glShaderSource(handle, 1, &source, nullptr);
        glCompileShader(handle);
        glGetShaderiv(handle, GL_COMPILE_STATUS, &success);

        if (!success)
        {
            glGetShaderInfoLog(handle, sizeof(msg), nullptr, msg);
            Logger::error("%u: %s\n", type, msg);
            glDeleteShader(handle);
            return 0;
        }

        return handle;
    }

}