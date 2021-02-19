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

#include <borealis/core/application.hpp>
#include <borealis/core/logger.hpp>
#include <borealis/platforms/glfw/glfw_video.hpp>

#define GLM_FORCE_PURE
#define GLM_ENABLE_EXPERIMENTAL
#include <glad/glad.h>

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/rotate_vector.hpp>
#include <glm/mat4x4.hpp>
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>

// nanovg implementation
#define NANOVG_GL3_IMPLEMENTATION
#include <nanovg-gl/nanovg_gl.h>

namespace brls
{

static void glfwWindowFramebufferSizeCallback(GLFWwindow* window, int width, int height)
{
    if (!width || !height)
        return;

    glViewport(0, 0, width, height);

    Application::onWindowResized(width, height);
}

GLFWVideoContext::GLFWVideoContext(std::string windowTitle, uint32_t windowWidth, uint32_t windowHeight)
{
    // Create window
#ifdef __APPLE__
    // Explicitly ask for a 3.2 context on OS X
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    // Force scaling off to keep desired framebuffer size
    glfwWindowHint(GLFW_COCOA_RETINA_FRAMEBUFFER, GLFW_FALSE);
#else
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
#endif

    this->window = glfwCreateWindow(windowWidth, windowHeight, windowTitle.c_str(), nullptr, nullptr);

    if (!this->window)
    {
        Logger::error("glfw: failed to create window");
        glfwTerminate();
        return;
    }

    // Configure window
    glfwSetInputMode(window, GLFW_STICKY_KEYS, GLFW_TRUE);
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, glfwWindowFramebufferSizeCallback);

    // Load OpenGL routines using glad
    gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);
    glfwSwapInterval(1);

    Logger::info("glfw: GL Vendor: {}", glGetString(GL_VENDOR));
    Logger::info("glfw: GL Renderer: {}", glGetString(GL_RENDERER));
    Logger::info("glfw: GL Version: {}", glGetString(GL_VERSION));

    // Initialize nanovg
    this->nvgContext = nvgCreateGL3(NVG_STENCIL_STROKES | NVG_ANTIALIAS);
    if (!this->nvgContext)
    {
        Logger::error("glfw: unable to init nanovg");
        glfwTerminate();
        return;
    }

    // Setup scaling
    glfwWindowFramebufferSizeCallback(window, windowWidth, windowHeight);
}

void GLFWVideoContext::beginFrame()
{
}

void GLFWVideoContext::endFrame()
{
    glfwSwapBuffers(this->window);
}

void GLFWVideoContext::clear(NVGcolor color)
{
    glClearColor(
        color.r,
        color.g,
        color.b,
        1.0f);

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
}

void GLFWVideoContext::resetState()
{
    glDisable(GL_CULL_FACE);
    glDisable(GL_BLEND);
    glDisable(GL_DEPTH_TEST);
    glDisable(GL_SCISSOR_TEST);
    glDisable(GL_STENCIL_TEST);
}

GLFWVideoContext::~GLFWVideoContext()
{
    if (this->nvgContext)
        nvgDeleteGL3(this->nvgContext);

    glfwTerminate();
}

NVGcontext* GLFWVideoContext::getNVGContext()
{
    return this->nvgContext;
}

GLFWwindow* GLFWVideoContext::getGLFWWindow()
{
    return this->window;
}

} // namespace brls
