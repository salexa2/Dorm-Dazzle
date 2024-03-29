#ifndef GUIMANAGER_H
#define GUIMANAGER_H
#include <imgui.h>
#include "imgui_impl_glfw.h"
#include <backends/imgui_impl_wgpu.h>
#include <backends/imgui_impl_glfw.h>
#include "glm/glm.hpp"
#include "GLFW/glfw3.h" //shouimplld we include this everywhere
#include "EntityManager.h"

class GuiManager
{

public:
        GuiManager();
        void Start(GLFWwindow *window, WGPUDevice device, WGPUTextureFormat swapchainformat);
        void Shutdown();
        void Draw(WGPURenderPassEncoder render_pass);
        void DrawMainMenu(WGPURenderPassEncoder render_pass);

        std::string curr_bed;     // bed
        std::string curr_lamp;    // lamp
        std::string curr_desk;    // desk
        std::string curr_dresser; // dresser
        std::string curr_fridge;  // fridge
        std::string curr_floor;   // floor
        std::string curr_wall;    // wall
        std::string curr_window;
        float maxStamina;
        float currentStamina;
        float replenish_rate;

        struct Item
        {
                std::string item_name;
                int price;
                bool purchaced;
        };

        std::vector<std::string> purchasedItems;

        // helper functions
        void SetTemp();
        void ResetBoring();
        void ResetGame();

        void loadPurchasedItems(const std::string &filename);
        void savePurchasedItems(const std::string &filename, const std::vector<std::string> &stringList);
        bool isPurchased(std::string item);
        void saveMoney(const std::string &filename);
        void loadMoney(const std::string &filename);

        void InitEnergyBar(); // public helper for energy bar
        void SaveTime();

private:
        float LoadEnergy();
        time_t LoadTime(); // helper function for reading last closed time
        void SaveEnergy(); // helper function for writing current energy at shutdown

        void SoundPicker(int val);

        void MenuButtons(std::vector<std::string> names, std::vector<int> indexs, int &selected_index);

        // Draw Helper Functions
        void EnergyDrawer();
        void DormShopDrawer();
        void DormShopSetter(const char *button_name, std::string item_name, std::string curritem, int price, int entitynum);
        void CheckHovered(const char *button_name, std::string item_name, std::string curritem, int price, int entitynum);
        void BreadDrawer();
        void InvDrawer();
};

#endif