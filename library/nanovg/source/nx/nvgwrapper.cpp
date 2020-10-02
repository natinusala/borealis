#include <switch.h>

#include <cstdio>
#include <cstring>
#include <deko3d.hpp>
#include <optional>
#include <nvgwrapper.hpp>

#include "framework/CMemPool.h"
#include "nanovg_dk.h"

namespace nvg {

    namespace {

        class DkTest {
          public:
            static constexpr const unsigned NumFramebuffers = 2;
            static constexpr const unsigned StaticCmdSize   = 0x1000;

            static constexpr const float DefaultWindowWidth  = 1280.f;
            static constexpr const float DefaultWindowHeight = 720.f;

            float framebuffer_width  = DefaultWindowWidth;
            float framebuffer_height = DefaultWindowHeight;
            float window_scale       = 1.f;

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
            NVGcontext *vg;

          public:
            DkTest() {
                this->UpdateFrameBufferSize();

                // Create the deko3d device
                device = dk::DeviceMaker {}
                             .create();

                // Create the main queue
                queue = dk::QueueMaker { device }.setFlags(DkQueueFlags_Graphics).create();

                // Create the memory pools
                pool_images.emplace(device, DkMemBlockFlags_GpuCached | DkMemBlockFlags_Image, 4 * 1024 * 1024);
                pool_code.emplace(device, DkMemBlockFlags_CpuUncached | DkMemBlockFlags_GpuCached | DkMemBlockFlags_Code, 128 * 1024);
                pool_data.emplace(device, DkMemBlockFlags_CpuUncached | DkMemBlockFlags_GpuCached, 1 * 1024 * 1024);

                // Create the static command buffer and feed it freshly allocated memory
                cmdbuf                  = dk::CmdBufMaker { device }.create();
                CMemPool::Handle cmdmem = pool_data->allocate(StaticCmdSize);
                cmdbuf.addMemory(cmdmem.getMemBlock(), cmdmem.getOffset(), cmdmem.getSize());

                // Create the framebuffer resources
                createFramebufferResources();

                this->renderer.emplace(framebuffer_width, framebuffer_height, this->device, this->queue, *this->pool_images, *this->pool_code, *this->pool_data);
                this->vg = nvgCreateDk(&*this->renderer, NVG_ANTIALIAS | NVG_STENCIL_STROKES);

                PlFontData font;
                int regular = -1;
                int symbols = -1;

                // Standard font
                if (R_SUCCEEDED(plGetSharedFontByType(&font, PlSharedFontType_Standard))) {
                    regular = nvgCreateFontMem(this->vg, "regular", (unsigned char *)font.address, font.size, false);
                }

                // Extented font
                if (R_SUCCEEDED(plGetSharedFontByType(&font, PlSharedFontType_NintendoExt))) {
                    symbols = nvgCreateFontMem(this->vg, "symbols", (unsigned char *)font.address, font.size, false);
                }

                nvgAddFallbackFontId(this->vg, regular, symbols);
            }

            ~DkTest() {
                // Destroy the framebuffer resources. This should be done first.
                destroyFramebufferResources();

                // Cleanup vg. This needs to be done first as it relies on the renderer.
                nvgDeleteDk(vg);

                // Destroy the renderer
                this->renderer.reset();
            }

            void createFramebufferResources() {
                // Create layout for the depth buffer
                dk::ImageLayout layout_depthbuffer;
                dk::ImageLayoutMaker { device }
                    .setFlags(DkImageFlags_UsageRender | DkImageFlags_HwCompression)
                    .setFormat(DkImageFormat_S8)
                    .setDimensions(framebuffer_width, framebuffer_height)
                    .initialize(layout_depthbuffer);

                // Create the depth buffer
                depthBuffer_mem = pool_images->allocate(layout_depthbuffer.getSize(), layout_depthbuffer.getAlignment());
                depthBuffer.initialize(layout_depthbuffer, depthBuffer_mem.getMemBlock(), depthBuffer_mem.getOffset());

                // Create layout for the framebuffers
                dk::ImageLayout layout_framebuffer;
                dk::ImageLayoutMaker { device }
                    .setFlags(DkImageFlags_UsageRender | DkImageFlags_UsagePresent | DkImageFlags_HwCompression)
                    .setFormat(DkImageFormat_BGR565_Unorm)
                    .setDimensions(framebuffer_width, framebuffer_height)
                    .initialize(layout_framebuffer);

                // Create the framebuffers
                std::array<DkImage const *, NumFramebuffers> fb_array;
                uint64_t fb_size  = layout_framebuffer.getSize();
                uint32_t fb_align = layout_framebuffer.getAlignment();
                for (unsigned i = 0; i < NumFramebuffers; i++) {
                    // Allocate a framebuffer
                    framebuffers_mem[i] = pool_images->allocate(fb_size, fb_align);
                    framebuffers[i].initialize(layout_framebuffer, framebuffers_mem[i].getMemBlock(), framebuffers_mem[i].getOffset());

                    // Generate a command list that binds it
                    dk::ImageView colorTarget { framebuffers[i] }, depthTarget { depthBuffer };
                    cmdbuf.bindRenderTargets(&colorTarget, &depthTarget);
                    framebuffer_cmdlists[i] = cmdbuf.finishList();

                    // Fill in the array for use later by the swapchain creation code
                    fb_array[i] = &framebuffers[i];
                }

                // Create the swapchain using the framebuffers
                swapchain = dk::SwapchainMaker { device, nwindowGetDefault(), fb_array }.create();

                // Generate the main rendering cmdlist
                recordStaticCommands();
            }

            void destroyFramebufferResources() {
                // Return early if we have nothing to destroy
                if (!swapchain)
                    return;

                // Make sure the queue is idle before destroying anything
                queue.waitIdle();

                // Clear the static cmdbuf, destroying the static cmdlists in the process
                cmdbuf.clear();

                // Destroy the swapchain
                swapchain.destroy();

                // Destroy the framebuffers
                for (unsigned i = 0; i < NumFramebuffers; i++)
                    framebuffers_mem[i].destroy();

                // Destroy the depth buffer
                depthBuffer_mem.destroy();
            }

            void recordStaticCommands() {
                // Initialize state structs with deko3d defaults
                dk::RasterizerState rasterizerState;
                dk::ColorState colorState;
                dk::ColorWriteState colorWriteState;
                dk::BlendState blendState;

                // Configure the viewport and scissor
                cmdbuf.setViewports(0, { { 0.0f, 0.0f, framebuffer_width, framebuffer_height, 0.0f, 1.0f } });
                cmdbuf.setScissors(0, { { 0, 0, static_cast<uint32_t>(framebuffer_width), static_cast<uint32_t>(framebuffer_height) } });

                // Clear the color and depth buffers
                cmdbuf.clearColor(0, DkColorMask_RGBA, 0.0f, 0.0f, 0.0f, 0.85f);
                cmdbuf.clearDepthStencil(true, 1.0f, 0xFF, 0);

                // Bind required state
                cmdbuf.bindRasterizerState(rasterizerState);
                cmdbuf.bindColorState(colorState);
                cmdbuf.bindColorWriteState(colorWriteState);

                render_cmdlist = cmdbuf.finishList();
            }

            int BeginFrame() {
                // Acquire a framebuffer from the swapchain (and wait for it to be available)
                int slot = queue.acquireImage(swapchain);

                // Run the command list that attaches said framebuffer to the queue
                queue.submitCommands(framebuffer_cmdlists[slot]);

                // Run the main rendering command list
                queue.submitCommands(render_cmdlist);

                nvgBeginFrame(vg, framebuffer_width, framebuffer_height, 1.0f);
                nvgScale(vg, window_scale, window_scale);

                return slot;
            }

            void EndFrame(int slot) {
                nvgResetTransform(vg);
                nvgEndFrame(vg);

                // Now that we are done rendering, present it to the screen
                queue.presentImage(swapchain, slot);
            }

            void UpdateFrameBufferSize() {
                switch (appletGetOperationMode()) {
                    case AppletOperationMode_Handheld:
                        framebuffer_width  = 1280.f;
                        framebuffer_height = 720.f;
                        break;
                    case AppletOperationMode_Docked:
                        framebuffer_width  = 1920.f;
                        framebuffer_height = 1080.f;
                        break;
                    default:
                        return;
                }

                window_scale = (float)framebuffer_width / (float)DefaultWindowWidth;

                renderer->UpdateViewBounds(framebuffer_width, framebuffer_height);
            }
        };

        std::optional<DkTest> s_Dk;
        AppletHookCookie s_cookie;
        u64 kDown, kUp, kHeld;
        TouchState touchState;
        touchPosition touchPos;
        bool running;

        void AppletCallback(AppletHookType hook, void *param) {
            switch (hook) {
                case AppletHookType_OnOperationMode:
                    s_Dk->destroyFramebufferResources();
                    s_Dk->UpdateFrameBufferSize();
                    s_Dk->createFramebufferResources();
                    break;
                default:
                    break;
            }

            (void)param;
        }

    }

    void Initialize() {
        plInitialize(PlServiceType_User);

        appletHook(&s_cookie, AppletCallback, nullptr);
        s_Dk.emplace();
        running = true;
    }

    void Exit() {
        running = false;
        s_Dk.reset();

        plExit();
    }

    bool MainLoop() {
        hidScanInput();

        kDown = 0;
        kUp   = 0;
        kHeld = 0;
        for (u32 input = 0; input < 10; ++input) {
            kDown |= hidKeysDown(static_cast<HidControllerID>(input));
            kUp |= hidKeysUp(static_cast<HidControllerID>(input));
            kHeld |= hidKeysHeld(static_cast<HidControllerID>(input));
        }

        if (hidTouchCount()) {
            hidTouchRead(&touchPos, 0);
            switch (touchState) {
                case TouchState::Touched:
                case TouchState::Held:
                    touchState = TouchState::Held;
                    break;
                default:
                    touchState = TouchState::Touched;
                    break;
            }
        } else {
            switch (touchState) {
                case TouchState::Touched:
                case TouchState::Held:
                    touchState = TouchState::Released;
                    break;
                case TouchState::Released:
                    std::memset(&touchPos, 0, sizeof(touchPos));
                    touchState = TouchState::None;
                    break;
                default:
                    /* ... */
                    break;
            }
        }

        return running && appletMainLoop();
    }

    void Break() {
        running = false;
    }

    NVGcontext *GetContext() {
        return s_Dk->vg;
    }

    Frame::Frame() {
        this->impl = s_Dk->vg;
        this->slot = s_Dk->BeginFrame();
    }

    Frame::~Frame() {
        s_Dk->EndFrame(this->slot);
    }

    namespace keys {

        uint64_t Down() {
            return kDown;
        }

        uint64_t Up() {
            return kUp;
        }

        uint64_t Held() {
            return kHeld;
        }

    }

    TouchState GetTouchState() {
        return touchState;
    }

    std::pair<int32_t, int32_t> GetTouchPos() {
        return { touchPos.px, touchPos.py };
    }

}
