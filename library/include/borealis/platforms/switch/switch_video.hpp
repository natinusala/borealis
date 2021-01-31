/*
    Borealis, a Nintendo Switch UI Library
    Copyright (C) 2020  Adubbz
    Copyright (C) 2020-2021  Luis Scheurenbrand
    Copyright (C) 2021  natinusala

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
