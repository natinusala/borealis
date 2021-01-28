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

#include <switch.h>

#include <borealis/core/application.hpp>
#include <borealis/platforms/switch/switch_video.hpp>

#define HANDHELD_WIDTH 1280.0f
#define HANDHELD_HEIGHT 720.0f

#define DOCKED_WIDTH 1920.f
#define DOCKED_HEIGHT 1080.f

#define IMAGES_POOL_SIZE 4 * 1024 * 1024
#define CODE_POOL_SIZE 128 * 1024
#define DATA_POOL_SIZE 1 * 1024 * 1024

namespace brls
{

static constexpr const unsigned STATIC_CMD_SIZE = 0x1000;

SwitchVideoContext::SwitchVideoContext()
{
    // Setup scaling
    this->updateWindowSize();

    // Init deko
    this->device = dk::DeviceMaker {}.create();
    this->queue  = dk::QueueMaker { this->device }.setFlags(DkQueueFlags_Graphics).create();

    this->imagesPool.emplace(device, DkMemBlockFlags_GpuCached | DkMemBlockFlags_Image, IMAGES_POOL_SIZE);
    this->codePool.emplace(device, DkMemBlockFlags_CpuUncached | DkMemBlockFlags_GpuCached | DkMemBlockFlags_Code, CODE_POOL_SIZE);
    this->dataPool.emplace(device, DkMemBlockFlags_CpuUncached | DkMemBlockFlags_GpuCached, DATA_POOL_SIZE);

    this->cmdbuf            = dk::CmdBufMaker { device }.create();
    CMemPool::Handle cmdmem = this->dataPool->allocate(STATIC_CMD_SIZE);
    cmdbuf.addMemory(cmdmem.getMemBlock(), cmdmem.getOffset(), cmdmem.getSize());

    // Create framebuffer resources
    this->createFramebufferResources();
}

void SwitchVideoContext::createFramebufferResources()
{
    // Create layout for the depth buffer
    dk::ImageLayout depthBufferLayout;
    dk::ImageLayoutMaker { this->device }
        .setFlags(DkImageFlags_UsageRender | DkImageFlags_HwCompression)
        .setFormat(DkImageFormat_S8)
        .setDimensions(this->framebufferWidth, this->framebufferHeight)
        .initialize(depthBufferLayout);

    // Create the depth buffer
    this->depthBufferHandle = this->imagesPool->allocate(depthBufferLayout.getSize(), depthBufferLayout.getAlignment());
    this->depthBuffer.initialize(depthBufferLayout, depthBufferHandle.getMemBlock(), depthBufferHandle.getOffset());

    // Create layout for the framebuffers
    dk::ImageLayout framebufferLayout;
    dk::ImageLayoutMaker { device }
        .setFlags(DkImageFlags_UsageRender | DkImageFlags_UsagePresent | DkImageFlags_HwCompression)
        .setFormat(DkImageFormat_BGR565_Unorm)
        .setDimensions(this->framebufferWidth, this->framebufferHeight)
        .initialize(framebufferLayout);

    // Create the framebuffers
    std::array<DkImage const*, FRAMEBUFFERS_COUNT> fbArray;
    uint64_t fbSize  = framebufferLayout.getSize();
    uint32_t fbAlign = framebufferLayout.getAlignment();

    for (unsigned i = 0; i < FRAMEBUFFERS_COUNT; i++)
    {
        // Allocate a framebuffer
        framebuffersHandles[i] = this->imagesPool->allocate(fbSize, fbAlign);
        this->framebuffers[i].initialize(framebufferLayout, framebuffersHandles[i].getMemBlock(), framebuffersHandles[i].getOffset());

        // Generate a command list that binds it
        dk::ImageView colorTarget { this->framebuffers[i] }, depthTarget { depthBuffer };
        this->cmdbuf.bindRenderTargets(&colorTarget, &depthTarget);
        framebuffersCmdLists[i] = this->cmdbuf.finishList();

        // Fill in the array for use later by the swapchain creation code
        fbArray[i] = &this->framebuffers[i];
    }

    // Create the swapchain using the framebuffers
    this->swapchain = dk::SwapchainMaker { this->device, nwindowGetDefault(), fbArray }.create();

    // Generate the main rendering cmdlist
    this->recordStaticCommands();

    // Create renderer and nvg context
    this->renderer.emplace(framebuffer_width, framebuffer_height, this->device, this->queue, *this->pool_images, *this->pool_code, *this->pool_data);
}

void SwitchVideoContext::recordStaticCommands()
{
    // Initialize state structs with deko3d defaults
    dk::RasterizerState rasterizerState;
    dk::ColorState colorState;
    dk::ColorWriteState colorWriteState;
    dk::BlendState blendState;

    // Configure the viewport and scissor
    this->cmdbuf.setViewports(0, { { 0.0f, 0.0f, this->framebufferWidth, this->framebufferHeight, 0.0f, 1.0f } });
    this->cmdbuf.setScissors(0, { { 0, 0, static_cast<uint32_t>(this->framebufferWidth), static_cast<uint32_t>(this->framebufferHeight) } });

    // Clear the color and depth buffers
    this->cmdbuf.clearColor(0, DkColorMask_RGBA, 0.0f, 0.0f, 0.0f, 1.0f);
    this->cmdbuf.clearDepthStencil(true, 1.0f, 0xFF, 0);

    // Bind required state
    this->cmdbuf.bindRasterizerState(rasterizerState);
    this->cmdbuf.bindColorState(colorState);
    this->cmdbuf.bindColorWriteState(colorWriteState);

    this->renderCmdlist = cmdbuf.finishList();
}

// TODO: trigger that when the console is docked
void SwitchVideoContext::updateWindowSize()
{
    // TODO: change that switch/case to something more future proof (maybe use GetDefaultDisplayResolution? don't forget it can fail on early fw)
    switch (appletGetOperationMode())
    {
        case AppletOperationMode_Handheld:
            this->framebufferWidth  = HANDHELD_WIDTH;
            this->framebufferHeight = HANDHELD_HEIGHT;
            break;
        case AppletOperationMode_Console:
            this->framebufferWidth  = DOCKED_WIDTH;
            this->framebufferHeight = DOCKED_HEIGHT;
            break;
        default:
            return;
    }

    Application::onWindowResized(roundf(this->framebufferWidth), roundf(this->framebufferHeight));
}

} // namespace brls
