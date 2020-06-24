#include <nanovg/framework/CApplication.h>
#include <nanovg/framework/CMemPool.h>

// C++ standard library headers
#include <array>
#include <optional>
#include <unistd.h>

#include "nanovg.h"
#include "nanovg_dk.h"
#include "demo.h"

// #define USE_OPENGL
#ifndef USE_OPENGL

//#define DEBUG_NXLINK

#ifdef DEBUG_NXLINK
static int nxlink_sock = -1;
#endif

extern "C" void userAppInit(void)
{
    Result res = romfsInit();
    if (R_FAILED(res))
        fatalThrow(res);

#ifdef DEBUG_NXLINK
    socketInitializeDefault();
    nxlink_sock = nxlinkStdioForDebug();
#endif
}

extern "C" void userAppExit(void)
{
#ifdef DEBUG_NXLINK
    if (nxlink_sock != -1)
        close(nxlink_sock);
    socketExit();
#endif

    romfsExit();
}

void OutputDkDebug(void* userData, const char* context, DkResult result, const char* message) 
{
    printf("Context: %s\nResult: %d\nMessage: %s\n", context, result, message);
}

class DkTest final : public CApplication
{
    static constexpr unsigned NumFramebuffers = 2;
    static constexpr uint32_t FramebufferWidth = 1280;
    static constexpr uint32_t FramebufferHeight = 720;
    static constexpr unsigned StaticCmdSize = 0x1000;

    dk::UniqueDevice device;
    dk::UniqueQueue queue;

    std::optional<CMemPool> pool_images;
    std::optional<CMemPool> pool_code;
    std::optional<CMemPool> pool_data;

    dk::UniqueCmdBuf cmdbuf;

    CMemPool::Handle depthBuffer_mem;
    CMemPool::Handle framebuffers_mem[NumFramebuffers];

    dk::Image depthBuffer;
    dk::Image framebuffers[NumFramebuffers];
    DkCmdList framebuffer_cmdlists[NumFramebuffers];
    dk::UniqueSwapchain swapchain;

    DkCmdList render_cmdlist;

    std::optional<nvg::DkRenderer> renderer;
    NVGcontext* vg;

    DemoData data;

public:
    DkTest()
    {
        // Create the deko3d device
        device = dk::DeviceMaker{}.setCbDebug(OutputDkDebug).create();

        // Create the main queue
        queue = dk::QueueMaker{device}.setFlags(DkQueueFlags_Graphics).create();

        // Create the memory pools
        pool_images.emplace(device, DkMemBlockFlags_GpuCached | DkMemBlockFlags_Image, 16*1024*1024);
        pool_code.emplace(device, DkMemBlockFlags_CpuUncached | DkMemBlockFlags_GpuCached | DkMemBlockFlags_Code, 128*1024);
        pool_data.emplace(device, DkMemBlockFlags_CpuUncached | DkMemBlockFlags_GpuCached, 1*1024*1024);

        // Create the static command buffer and feed it freshly allocated memory
        cmdbuf = dk::CmdBufMaker{device}.create();
        CMemPool::Handle cmdmem = pool_data->allocate(StaticCmdSize);
        cmdbuf.addMemory(cmdmem.getMemBlock(), cmdmem.getOffset(), cmdmem.getSize());

        // Create the framebuffer resources
        createFramebufferResources();

        this->renderer.emplace(FramebufferWidth, FramebufferHeight, this->device, this->queue, *this->pool_images, *this->pool_code, *this->pool_data);
        this->vg = nvgCreateDk(&*this->renderer, NVG_ANTIALIAS | NVG_STENCIL_STROKES);

        if (loadDemoData(vg, &this->data) == -1) {
            printf("Failed to load demo data!\n");
        }
    }

    ~DkTest()
    {
        freeDemoData(vg, &this->data);

        // Cleanup vg. This needs to be done first as it relies on the renderer.
        nvgDeleteDk(vg);

        // Destroy the renderer
        this->renderer.reset();

        // Destroy the framebuffer resources
        destroyFramebufferResources();
    }

    void createFramebufferResources()
    {
      // Create layout for the depth buffer
        dk::ImageLayout layout_depthbuffer;
        dk::ImageLayoutMaker{device}
            .setFlags(DkImageFlags_UsageRender | DkImageFlags_HwCompression)
            .setFormat(DkImageFormat_S8)
            .setDimensions(FramebufferWidth, FramebufferHeight)
            .initialize(layout_depthbuffer);

        // Create the depth buffer
        depthBuffer_mem = pool_images->allocate(layout_depthbuffer.getSize(), layout_depthbuffer.getAlignment());
        depthBuffer.initialize(layout_depthbuffer, depthBuffer_mem.getMemBlock(), depthBuffer_mem.getOffset());

        // Create layout for the framebuffers
        dk::ImageLayout layout_framebuffer;
        dk::ImageLayoutMaker{device}
            .setFlags(DkImageFlags_UsageRender | DkImageFlags_UsagePresent | DkImageFlags_HwCompression)
            .setFormat(DkImageFormat_RGBA8_Unorm)
            .setDimensions(FramebufferWidth, FramebufferHeight)
            .initialize(layout_framebuffer);

        // Create the framebuffers
        std::array<DkImage const*, NumFramebuffers> fb_array;
        uint64_t fb_size  = layout_framebuffer.getSize();
        uint32_t fb_align = layout_framebuffer.getAlignment();
        for (unsigned i = 0; i < NumFramebuffers; i ++)
        {
            // Allocate a framebuffer
            framebuffers_mem[i] = pool_images->allocate(fb_size, fb_align);
            framebuffers[i].initialize(layout_framebuffer, framebuffers_mem[i].getMemBlock(), framebuffers_mem[i].getOffset());

            // Generate a command list that binds it
            dk::ImageView colorTarget{ framebuffers[i] }, depthTarget{ depthBuffer };
            cmdbuf.bindRenderTargets(&colorTarget, &depthTarget);
            framebuffer_cmdlists[i] = cmdbuf.finishList();

            // Fill in the array for use later by the swapchain creation code
            fb_array[i] = &framebuffers[i];
        }

        // Create the swapchain using the framebuffers
        swapchain = dk::SwapchainMaker{device, nwindowGetDefault(), fb_array}.create();

        // Generate the main rendering cmdlist
        recordStaticCommands();
    }

    void destroyFramebufferResources()
    {
        // Return early if we have nothing to destroy
        if (!swapchain) return;

        // Make sure the queue is idle before destroying anything
        queue.waitIdle();

        // Clear the static cmdbuf, destroying the static cmdlists in the process
        cmdbuf.clear();

        // Destroy the swapchain
        swapchain.destroy();

        // Destroy the framebuffers
        for (unsigned i = 0; i < NumFramebuffers; i ++)
            framebuffers_mem[i].destroy();

        // Destroy the depth buffer
        depthBuffer_mem.destroy();
    }

    void recordStaticCommands()
    {
        // Initialize state structs with deko3d defaults
        dk::RasterizerState rasterizerState;
        dk::ColorState colorState;
        dk::ColorWriteState colorWriteState;
        dk::BlendState blendState;

        // Configure the viewport and scissor
        cmdbuf.setViewports(0, { { 0.0f, 0.0f, FramebufferWidth, FramebufferHeight, 0.0f, 1.0f } });
        cmdbuf.setScissors(0, { { 0, 0, FramebufferWidth, FramebufferHeight } });

        // Clear the color and depth buffers
        cmdbuf.clearColor(0, DkColorMask_RGBA, 0.2f, 0.3f, 0.3f, 1.0f);
        cmdbuf.clearDepthStencil(true, 1.0f, 0xFF, 0);

        // Bind required state
        cmdbuf.bindRasterizerState(rasterizerState);
        cmdbuf.bindColorState(colorState);
        cmdbuf.bindColorWriteState(colorWriteState);

        render_cmdlist = cmdbuf.finishList();
    }

    void render(u64 ns, int blowup)
    {
        // Acquire a framebuffer from the swapchain (and wait for it to be available)
        int slot = queue.acquireImage(swapchain);

        // Run the command list that attaches said framebuffer to the queue
        queue.submitCommands(framebuffer_cmdlists[slot]);

        // Run the main rendering command list
        queue.submitCommands(render_cmdlist);

        nvgBeginFrame(vg, FramebufferWidth, FramebufferHeight, 1.0f);
        {
            // Render stuff!
            renderDemo(vg, 0, 0, FramebufferWidth, FramebufferHeight, static_cast<float>(ns) / 1000'000'000.0f, blowup, &this->data);
        }
        nvgEndFrame(vg);

        // Now that we are done rendering, present it to the screen
        queue.presentImage(swapchain, slot);
    }

    bool onFrame(u64 ns) override
    {
        hidScanInput();
        u64 kDown = hidKeysDown(CONTROLLER_P1_AUTO);
        u64 kHeld = hidKeysHeld(CONTROLLER_P1_AUTO);
        if (kDown & KEY_PLUS)
            return false;

        render(ns, kHeld & KEY_MINUS);
        return true;
    }
};

// Main entrypoint
int main(int argc, char* argv[])
{
    DkTest app;
    app.run();
    return 0;
}
#endif