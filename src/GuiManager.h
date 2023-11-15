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
                std::string temp3; //desk
                std::string temp4; //dresser
                std::string temp5; //fridge
                std::string temp6; //floor
                float maxStamina;
                float currentStamina;
                float replenish_rate;

                std::vector<std::string> purchasedItems;
               


                void SetTemp();

                void loadPurchasedItems(const std::string& filename);
                void savePurchasedItems(const std::string& filename, const std::vector<std::string>& stringList);
                bool isPurchased(std::string item); 
                void saveMoney(const std::string& filename);
                void loadMoney(const std::string& filename);

                void InitEnergyBar(); //public helper for energy bar
                void SaveTime();
                float LoadEnergy();
                time_t LoadTime(); //helper function for reading last closed time
                void SaveEnergy(); //helper function for writing current energy at shutdown


        private:
              





};

#endif