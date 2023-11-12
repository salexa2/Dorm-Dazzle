#ifndef GUIMANAGER_H
#define GUIMANAGER_H
#include <imgui.h>
#include "imgui_impl_glfw.h"
#include <backends/imgui_impl_wgpu.h>
#include <backends/imgui_impl_glfw.h>
#include "glm/glm.hpp"
#include "GLFW/glfw3.h" //shouimplld we include this everywhere
#include "EntityManager.h"



class GuiManager{

public:
        GuiManager();
        void Start( GLFWwindow* window, WGPUDevice device,  WGPUTextureFormat swapchainformat);
        void Shutdown();
        void Draw(WGPURenderPassEncoder render_pass);
        std::string temp;//bed
        std::string temp2; //lamp

        void SetTemp();

private:
    time_t LoadTime(); //helper function for reading last closed time
    void SaveEnergy(); //helper function for writing current energy at shutdown

    //make these public for... reasons lol
    float maxStamina;
    float currentStamina;
    float replenish_rate;
    
};

#endif