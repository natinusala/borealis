/*
    Copyright 2020 Adubbz
    Copyright 2020-2021 Luis Scheurenbrand
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

#pragma once

#include <nanovg/framework/CMemPool.h>
#include <switch.h>

#include <borealis/core/video.hpp>
#include <deko3d.hpp>
#include <nanovg/dk_renderer.hpp>
#include <optional>

typedef Event _LibNXEvent; // "Event" alone clashes with brls::Event

namespace brls
{

static constexpr const unsigned FRAMEBUFFERS_COUNT = 2;

// deko3d video context
class SwitchVideoContext : public VideoContext
{
  public:
    SwitchVideoContext();
    ~SwitchVideoContext();

    void clear(NVGcolor color) override;
    void resetState() override;
    void beginFrame() override;
    void endFrame() override;
    virtual NVGcontext* getNVGContext() override;

    void appletCallback(AppletHookType hookType);

  private:
    _LibNXEvent defaultDisplayResolutionChangeEvent;
    bool displayResolutionChangeEventReady = true;

    void resetFramebuffer(); // triggered by either display resolution change event or operation mode change event
    void updateWindowSize();
    void createFramebufferResources();
    void recordStaticCommands();

    void destroyFramebufferResources();

    float framebufferWidth, framebufferHeight;
    int imageSlot;

    dk::UniqueDevice device;
    dk::UniqueQueue queue;
    dk::UniqueCmdBuf cmdbuf;
    dk::UniqueSwapchain swapchain;

    dk::Image depthBuffer;
    CMemPool::Handle depthBufferHandle;

    dk::Image framebuffers[FRAMEBUFFERS_COUNT];
    CMemPool::Handle framebuffersHandles[FRAMEBUFFERS_COUNT];

    DkCmdList framebuffersCmdLists[FRAMEBUFFERS_COUNT];
    DkCmdList renderCmdlist;

    std::optional<CMemPool> imagesPool;
    std::optional<CMemPool> codePool;
    std::optional<CMemPool> dataPool;

    std::optional<nvg::DkRenderer> renderer;
    NVGcontext* nvgContext;
};

} // namespace brls
