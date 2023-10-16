#ifndef GUIMANAGER_H
#define GUIMANAGER_H
#include <imgui.h>
#include <backends/imgui_impl_wgpu.h>
#include <backends/imgui_impl_glfw.h>


class GuiManager{

        public:
                GuiManager();
                void Start( GLFWwindow* window, WGPUDevice device,  WGPUTextureFormat swapchainformat);
                void Shutdown();
                void Draw();


        private:
              





};

#endif