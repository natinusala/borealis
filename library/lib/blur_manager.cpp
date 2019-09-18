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

#include <borealis.hpp>

#include <fstream>
#include <sstream>

namespace brls
{

    void BlurManager::initBlurring()
    {   
        std::stringstream blurVertShaderSrc, blurFragShaderSrc;
        blurVertShaderSrc << std::ifstream(ASSET("shaders/gaussian_blur/vert.glsl")).rdbuf();
        blurFragShaderSrc << std::ifstream(ASSET("shaders/gaussian_blur/frag.glsl")).rdbuf();

        GLint vsh = ShaderUtils::createAndCompileShader(GL_VERTEX_SHADER, blurVertShaderSrc.str().c_str());
        GLint fsh = ShaderUtils::createAndCompileShader(GL_FRAGMENT_SHADER, blurFragShaderSrc.str().c_str());

        BlurManager::shaderProgram = glCreateProgram();
        glAttachShader(BlurManager::shaderProgram, vsh);
        glAttachShader(BlurManager::shaderProgram, fsh);
        glLinkProgram(BlurManager::shaderProgram);
        glValidateProgram(BlurManager::shaderProgram);

        GLint success;
        glGetProgramiv(BlurManager::shaderProgram, GL_LINK_STATUS, &success);
        if (!success)
        {
            char buf[512];
            glGetProgramInfoLog(BlurManager::shaderProgram, sizeof(buf), nullptr, buf);
            Logger::error("Link error: %s", buf);
        }
        glDeleteShader(vsh);
        glDeleteShader(fsh);

        glGenVertexArrays(1, &vao);
        glGenBuffers(1, &vbo);
        glBindVertexArray(vao);

        glBindBuffer(GL_ARRAY_BUFFER, vbo);
        glBufferData(GL_ARRAY_BUFFER, sizeof(ShaderUtils::rectVertices), ShaderUtils::rectVertices, GL_STATIC_DRAW);

        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(ShaderUtils::Vertex), (void*)offsetof(ShaderUtils::Vertex, position));
        glEnableVertexAttribArray(0);

        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(ShaderUtils::Vertex), (void*)offsetof(ShaderUtils::Vertex, color));
        glEnableVertexAttribArray(1);

        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindVertexArray(0);
    }

    void BlurManager::exitBlurring()
    {
        glBindVertexArray(0);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glUseProgram(0);
        
        glDeleteProgram(BlurManager::shaderProgram);
        glDeleteBuffers(1, &vao);
        glDeleteBuffers(1, &vbo);
    }


    bool BlurManager::createFramebuffers(GLuint width, GLuint height)
    {
        if (BlurManager::framebufferA || BlurManager::framebufferB)
            return true;

        BlurManager::framebufferA = nvgluCreateFramebuffer(Application::getNVGContext(), width, height, NVG_IMAGE_REPEATX | NVG_IMAGE_REPEATY);
        BlurManager::framebufferB = nvgluCreateFramebuffer(Application::getNVGContext(), width, height, NVG_IMAGE_REPEATX | NVG_IMAGE_REPEATY);
        
        return BlurManager::framebufferA && BlurManager::framebufferB;
    }

    void BlurManager::deleteFramebuffers()
    {
        if (BlurManager::framebufferA)
            nvgluDeleteFramebuffer(BlurManager::framebufferA);
        if (BlurManager::framebufferB)
            nvgluDeleteFramebuffer(BlurManager::framebufferB);   

        BlurManager::framebufferA = nullptr;
        BlurManager::framebufferB = nullptr;
    }


    void BlurManager::enableBlur()
    {
        nvgluBindFramebuffer(BlurManager::framebufferA);
    }

    void BlurManager::disableBlur()
    {
        nvgluBindFramebuffer(nullptr);
    }

    void BlurManager::applyBlur(FrameContext frameContext, GLuint windowWidth, GLuint windowHeight, unsigned numIterations)
    {
        BlurManager::readBuffer = framebufferA;
        BlurManager::writeBuffer = framebufferB;

        glDisable(GL_CULL_FACE);
        glDisable(GL_BLEND);
        glDisable(GL_DEPTH_TEST);
        glDisable(GL_SCISSOR_TEST);
        glDisable(GL_STENCIL_TEST);


        glBindBuffer(GL_ARRAY_BUFFER, vbo);
        glBindVertexArray(vao);

        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(ShaderUtils::Vertex), (void*)offsetof(ShaderUtils::Vertex, position));
        glEnableVertexAttribArray(0);

        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(ShaderUtils::Vertex), (void*)offsetof(ShaderUtils::Vertex, color));
        glEnableVertexAttribArray(1);
        glUseProgram(BlurManager::shaderProgram);

        GLint textureLoc     = glGetUniformLocation(BlurManager::shaderProgram, "texture");

        GLint iResolutionLoc = glGetUniformLocation(BlurManager::shaderProgram, "iResolution");
        GLint flipLoc        = glGetUniformLocation(BlurManager::shaderProgram, "flip");
        GLint directionLoc   = glGetUniformLocation(BlurManager::shaderProgram, "direction");
        GLint blurLoc        = glGetUniformLocation(BlurManager::shaderProgram, "doesBlur");

        glUniform1i(textureLoc, 0);
        glUniform1i(blurLoc, 1);

        for (unsigned int i = 0; i < numIterations; i++)
        {
            glBindTexture(GL_TEXTURE_2D, readBuffer->texture);
            nvgluBindFramebuffer(writeBuffer);
            glUniform2f(iResolutionLoc, static_cast<float>(windowWidth), static_cast<float>(windowHeight));
            glUniform1i(flipLoc, 1);

            float radius = (numIterations - i - 1);
            glUniform2f(directionLoc, i % 2 == 0 ? radius : 0, i % 2 == 0 ? 0 : radius);
            glDrawArrays(GL_TRIANGLE_STRIP, 0, 3);
            NVGLUframebuffer* tmp = readBuffer;
            readBuffer            = writeBuffer;
            writeBuffer           = tmp;
        }

        glUniform2f(directionLoc, 0.0F, 0.0F);
        glUniform1i(flipLoc, 0);
        glUniform1i(blurLoc, 0);


        nvgluBindFramebuffer(nullptr);
        nvgBeginFrame(Application::getNVGContext(), windowWidth, windowHeight, frameContext.pixelRatio);
        nvgScale(Application::getNVGContext(), Application::windowScale, Application::windowScale);
        glBindTexture(GL_TEXTURE_2D, readBuffer->texture);

        glDrawArrays(GL_TRIANGLE_STRIP, 0, 3);

        glBindTexture(GL_TEXTURE_2D, 0);
    }

}