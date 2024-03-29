#ifndef GRAPHICSMANAGER_H
#define GRAPHICSMANAGER_H
#include "GLFW/glfw3.h" //should we include this everywhere
#include <webgpu/webgpu.h>
#include <glfw3webgpu.h>
#include <string>
#include <vector>
#include "glm/glm.hpp"
#include "webgpu_raii.h"
#include <unordered_map>
#include "EntityManager.h"
#include "GuiManager.h"
#include <nlohmann/json.hpp>
using json = nlohmann::json;

class InputManager;
class GraphicsManager
{

public:
    // Constructor
    GraphicsManager();
    // ~GraphicsManager();
    //----------Header Structs--------------

    bool playMode; //sets which gui menu's should be showing

    struct ImageData
    {
        int width;
        int height;
        WGPUTexture texture;
        //  ImageData();
        // ImageData() : width(0), height(0), texture(nullptr) {}

        ~ImageData()
        {
            if (texture)
            {
                wgpuTextureDestroy(texture);
                wgpuTextureRelease(texture);
            }
        }
    };

    struct Sprite
    {
        std::string image_name;
        glm::vec3 position;
        glm::vec2 scale;
        // long z;
        double z;
        // ImageData d;
    };

    //-----------FUNCTIONS--------------
    void Start();
    void Shutdown();
    //---initalize each instance variable(this makes it easy to debug)
    // WEBAPI ITSELF
    void InitInstance();
    // INFORMATION ABOUT THE WINDOW WE WANT TO DRAW INTO
    void InitSurface();
    // A GPU
    void InitAdapter();
    // CONFIGURED GPU READY TO USE
    void InitDevice();
    // QUES COMMANDS TO EXECUTE ON GPU
    void InitQueue();
    void InitVertexB();
    void InitUniformB();
    void InitChain();
    void InitSample();
    void InitShadeM();
    void InitPipe();

    bool LoadTexture(const std::string &name, const std::string &path);
    void Draw();
    json SerializeData();
    void SaveSprites(const std::string &filename);
    void LoadSprite(const std::string &filename);

    //  bool lImage(const std::string& name, const std::string& path );
    //
    //----------INSTANCE VARIABLES--------------
    GLFWwindow *window;
    // Checkpoint 4
    WGPUInstanceRef instance;
    WGPUSurfaceRef surface;
    WGPUAdapterRef adapter;
    WGPUDeviceRef device;
    WGPUQueueRef queue;
    WGPUBufferRef vertex_buffer;
    WGPUBufferRef uniform_buffer;
    WGPUTextureFormat swap_chain_format;
    WGPUSwapChainRef swapchain;
    WGPURenderPipelineRef pipeline;
    WGPUSamplerRef textSampler;
    WGPUShaderModuleRef shader_module;
    WGPURenderPassEncoderRef render_pass;
    std::unordered_map<std::string, ImageData> image_map;
    GuiManager guiManager;
    int width;
    int height;
    /*


    WGPUBuffer instance_buffer;*/

private:
    friend class InputManager;

    // whats a friend class
    // Other instance variables
};
#endif
