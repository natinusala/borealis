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

#include <borealis/core/platform.hpp>

#ifdef __SWITCH__
#include <borealis/platforms/switch/switch_platform.hpp>
#endif

#ifdef __GLFW__
#include <borealis/platforms/glfw/glfw_platform.hpp>
#endif

namespace brls
{

Platform* Platform::createPlatform()
{
#if defined(__SWITCH__)
    return new SwitchPlatform();
#elif defined(__GLFW__)
    return new GLFWPlatform();
#endif

    return nullptr;
}

} // namespace brls
