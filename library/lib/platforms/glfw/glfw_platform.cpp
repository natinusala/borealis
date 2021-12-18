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
#include <borealis/core/i18n.hpp>
#include <borealis/core/logger.hpp>
#include <borealis/platforms/glfw/glfw_platform.hpp>
#include <locale>
#include <algorithm>

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

// glfw video and input code inspired from the glfw hybrid app by fincs
// https://github.com/fincs/hybrid_app

namespace brls
{

static void glfwErrorCallback(int errorCode, const char* description)
{
    Logger::error("glfw: error {}: {}", errorCode, description);
}

GLFWPlatform::GLFWPlatform()
{
    // Init glfw
    glfwSetErrorCallback(glfwErrorCallback);
    glfwInitHint(GLFW_JOYSTICK_HAT_BUTTONS, GLFW_TRUE);

    if (!glfwInit())
    {
        Logger::error("glfw: failed to initialize");
        return;
    }

    // Misc
    glfwSetTime(0.0);

    // Platform impls
    this->fontLoader  = new GLFWFontLoader();
    this->audioPlayer = new NullAudioPlayer();

    if (Application::getUsingi18n())
    {
        // Get platform's locale (taken from a comment here: https://stackoverflow.com/questions/32931458/getting-the-system-language-in-c-or-c/32931505#32931505)
        setlocale(LC_ALL, "");
        this->locale = std::string(setlocale(LC_ALL, NULL));

        // Split the default encoding and change the _ to - in locale (taken from my own testing on macOS Big Sur 11.0.1)
        this->locale = this->locale.substr(0, this->locale.find('.'));
        this->locale.replace(this->locale.find('_'), 1, "-");

        if ( std::find(LOCALE_LIST.begin(), LOCALE_LIST.end(), this->locale) == LOCALE_LIST.end() )
        {
            Logger::warning("Detected incompatible locale {}. Using default locale (en-US)...", this->locale);
            this->locale = LOCALE_DEFAULT;
        }
    }
    else
    {
        this->locale = LOCALE_DEFAULT;
    }
}

void GLFWPlatform::createWindow(std::string windowTitle, uint32_t windowWidth, uint32_t windowHeight)
{
    this->videoContext = new GLFWVideoContext(windowTitle, windowWidth, windowHeight);
    this->inputManager = new GLFWInputManager(this->videoContext->getGLFWWindow());
}

std::string GLFWPlatform::getName()
{
    return "GLFW";
}

bool GLFWPlatform::mainLoopIteration()
{
    bool isActive;
    do
    {
        isActive = !glfwGetWindowAttrib(this->videoContext->getGLFWWindow(), GLFW_ICONIFIED);

        if (isActive)
            glfwPollEvents();
        else
            glfwWaitEvents();
    } while (!isActive);

    return !glfwWindowShouldClose(this->videoContext->getGLFWWindow());
}

AudioPlayer* GLFWPlatform::getAudioPlayer()
{
    return this->audioPlayer;
}

VideoContext* GLFWPlatform::getVideoContext()
{
    return this->videoContext;
}

InputManager* GLFWPlatform::getInputManager()
{
    return this->inputManager;
}

FontLoader* GLFWPlatform::getFontLoader()
{
    return this->fontLoader;
}

ThemeVariant GLFWPlatform::getThemeVariant()
{
    char* themeEnv = getenv("BOREALIS_THEME");
    if (themeEnv != nullptr && !strcasecmp(themeEnv, "DARK"))
        return ThemeVariant::DARK;
    else
        return ThemeVariant::LIGHT;
}

std::string GLFWPlatform::getLocale()
{
    return this->locale;
}

GLFWPlatform::~GLFWPlatform()
{
    delete this->audioPlayer;
    delete this->videoContext;
    delete this->inputManager;
}

} // namespace brls
