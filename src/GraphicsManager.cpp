
//#define GLFW_INCLUDE_NONE
#include <iostream>
#include "GraphicsManager.h"
//Its fine, xmake is c

#include <spdlog/spdlog.h>
#include "glm/glm.hpp"
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#include <algorithm>
#include <fstream>



//-------------------GLOBAL VAR/STRUCTS/OTHER---------------------------
template< typename T > constexpr const T* to_ptr( const T& val ) { return &val; }
template< typename T, std::size_t N > constexpr const T* to_ptr( const T (&&arr)[N] ) { return arr; }


namespace{
    //A vertex buffer containing a textured square.
    
    const struct{
        // position
        float x, y;
        // texcoords
        float u, v;
    } vertices[] = {
        // position       // texcoords
        { -1.0f,  -1.0f,    0.0f,  1.0f },
        {  1.0f,  -1.0f,    1.0f,  1.0f },
        { -1.0f,   1.0f,    0.0f,  0.0f },
        {  1.0f,   1.0f,    1.0f,  0.0f },
    };
    //use this to avoid error
    using namespace glm;

    struct InstanceData {
        glm::vec3 translation;
        glm::vec2 scale;
        
        // rotation?
    };

    struct Uniforms {
       glm::mat4 projection;

       

    };
   
}

Uniforms uniforms;

std::string saveFilePath = "sprites.json"; 

const char* source = R"(
struct Uniforms {
    projection: mat4x4f,
};

@group(0) @binding(0) var<uniform> uniforms: Uniforms;
@group(0) @binding(1) var texSampler: sampler;
@group(0) @binding(2) var texData: texture_2d<f32>;

struct VertexInput {
    @location(0) position: vec2f,
    @location(1) texcoords: vec2f,
    @location(2) translation: vec3f,
    @location(3) scale: vec2f,
};

struct VertexOutput {
    @builtin(position) position: vec4f,
    @location(0) texcoords: vec2f,
};

@vertex
fn vertex_shader_main( in: VertexInput ) -> VertexOutput {
    var out: VertexOutput;
    out.position = uniforms.projection * vec4f( vec3f( in.scale * in.position, 0.0 ) + in.translation, 1.0 );
    out.texcoords = in.texcoords;
    return out;
}

@fragment
fn fragment_shader_main( in: VertexOutput ) -> @location(0) vec4f {
    let color = textureSample( texData, texSampler, in.texcoords ).rgba;
    return color;
})";











 
 
 
 
 //------------FUNCTIONS---------------------------
 GraphicsManager::GraphicsManager(){
   

}


void GraphicsManager::Start(){ 
    glfwInit();
    // We don't want GLFW to set up a graphics API.
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    //Checkpoint 4, prevent resizing window from breaking the thing.
    glfwWindowHint( GLFW_RESIZABLE, GLFW_FALSE );
    // Create the window.
    window = glfwCreateWindow(1600,900, "Sigma Engine", false ? glfwGetPrimaryMonitor() : 0, 0 );
    glfwSetWindowAspectRatio( window, 1600, 900); //900 800
    if( !window )
    {
        std::cerr << "Failed to create a window." << std::endl;
        glfwTerminate();
    }
    //----------- it will initialize the WebGPU API-----------
    //*Tips*:Make sure you initalize each variable, if you dont and miss one you might get a backtrace error when runninh
    // example: if you forget to initalize queue //InitQueue(); but initalize vertexB(); and other stuff after it, it will call back trace.
   
    InitInstance(); //1)
    InitSurface();//2)
    InitAdapter();//3)
    InitDevice();//4)
    wgpuDeviceSetUncapturedErrorCallback(
    device,
    []( WGPUErrorType type, char const* message, void* ) {
        std::cerr << "WebGPU uncaptured error type " << type << " with message: " << message << std::endl;
    },
    nullptr
    );

    InitQueue();
    InitVertexB(); //7
    
    wgpuQueueWriteBuffer( queue, vertex_buffer, 0, vertices, sizeof(vertices) ); //8
    swap_chain_format= wgpuSurfaceGetPreferredFormat( surface, adapter );
    glfwGetFramebufferSize( window, &width, &height );
    InitChain(); 
  // -------Compile shader programs--------------

   InitUniformB(); //10
   InitSample(); //11
   InitShadeM();
   //--------Pipeline----------
   InitPipe();
   
}
void GraphicsManager::Shutdown(){

     SaveSprites(saveFilePath);
     guiManager.Shutdown();
     glfwTerminate();
 
    
}
void GraphicsManager::InitInstance()
{
    instance = wgpuCreateInstance( to_ptr( WGPUInstanceDescriptor{} ) );

    if (instance == nullptr){
        glfwTerminate();
    }
}
void GraphicsManager::InitSurface()
{
    surface = glfwGetWGPUSurface( instance, window );
}
void GraphicsManager::InitAdapter()
{
    adapter = nullptr;
    wgpuInstanceRequestAdapter(
    instance,
    to_ptr( WGPURequestAdapterOptions{ .compatibleSurface = surface } ),
    []( WGPURequestAdapterStatus status, WGPUAdapter adapter, char const* message, void* adapter_ptr ) {
        if( status != WGPURequestAdapterStatus_Success ) {
            std::cerr << "Failed to get a WebGPU adapter: " << message << std::endl;
            glfwTerminate();
        }
        
        *static_cast<WGPUAdapter*>(adapter_ptr) = adapter;
    },
    &(adapter)
    );
}
void GraphicsManager::InitDevice()
{
device = nullptr;
wgpuAdapterRequestDevice(
    adapter,
    nullptr,
    []( WGPURequestDeviceStatus status, WGPUDevice device, char const* message, void* device_ptr ) {
        if( status != WGPURequestDeviceStatus_Success ) {
            std::cerr << "Failed to get a WebGPU device: " << message << std::endl;
            glfwTerminate();
        }
        
        *static_cast<WGPUDevice*>(device_ptr) = device;
    },
    &(device)
);
}
void GraphicsManager::InitQueue()
{
    queue = wgpuDeviceGetQueue( device );
}
void GraphicsManager::InitVertexB()
{
    vertex_buffer = wgpuDeviceCreateBuffer( device, to_ptr( WGPUBufferDescriptor{
    .usage = WGPUBufferUsage_CopyDst | WGPUBufferUsage_Vertex,
    .size = sizeof(vertices)
    }) );
}
void  GraphicsManager::InitUniformB()
{
     uniform_buffer = wgpuDeviceCreateBuffer( device, to_ptr( WGPUBufferDescriptor{
    .usage = WGPUBufferUsage_CopyDst | WGPUBufferUsage_Uniform,
    .size = sizeof(Uniforms)
    }) );
     
}
void GraphicsManager::InitChain()
{
   
    swapchain = wgpuDeviceCreateSwapChain( device, surface, to_ptr( WGPUSwapChainDescriptor{
        .usage = WGPUTextureUsage_RenderAttachment,
        .format = swap_chain_format,
        .width = (uint32_t)width,
        .height = (uint32_t)height
        }) );
        
}
void GraphicsManager::InitSample()
{
    textSampler= wgpuDeviceCreateSampler( device, to_ptr( WGPUSamplerDescriptor{
    .addressModeU = WGPUAddressMode_ClampToEdge,
    .addressModeV = WGPUAddressMode_ClampToEdge,
    .magFilter = WGPUFilterMode_Linear,
    .minFilter = WGPUFilterMode_Linear,
    .maxAnisotropy = 1
    } ) );
}
void GraphicsManager::InitShadeM()
{
    WGPUShaderModuleWGSLDescriptor code_desc = {};
    code_desc.chain.sType = WGPUSType_ShaderModuleWGSLDescriptor;
    code_desc.code = source; // The shader source as a `char*`
    WGPUShaderModuleDescriptor shader_desc = {};
    shader_desc.nextInChain = &code_desc.chain;
    shader_module = wgpuDeviceCreateShaderModule( device, &shader_desc );

}
void GraphicsManager::InitPipe()
{
     pipeline = wgpuDeviceCreateRenderPipeline( device, to_ptr( WGPURenderPipelineDescriptor{
    
    // Describe the vertex shader inputs
    .vertex = {
        .module = shader_module,
        .entryPoint = "vertex_shader_main",
        // Vertex attributes.
        .bufferCount = 2,
        .buffers = to_ptr<WGPUVertexBufferLayout>({
            // We have one buffer with our per-vertex position and UV data. This data never changes.
            // Note how the type, byte offset, and stride (bytes between elements) exactly matches our `vertex_buffer`.
            {
                .arrayStride = 4*sizeof(float),
                .attributeCount = 2,
                .attributes = to_ptr<WGPUVertexAttribute>({
                    // Position x,y are first.
                    {
                        .format = WGPUVertexFormat_Float32x2,
                        .offset = 0,
                        .shaderLocation = 0
                    },
                    // Texture coordinates u,v are second.
                    {
                        .format = WGPUVertexFormat_Float32x2,
                        .offset = 2*sizeof(float),
                        .shaderLocation = 1
                    }
                    })
            },
            // We will use a second buffer with our per-sprite translation and scale. This data will be set in our draw function.
            {
                .arrayStride = sizeof(InstanceData),
                // This data is per-instance. All four vertices will get the same value. Each instance of drawing the vertices will get a different value.
                // The type, byte offset, and stride (bytes between elements) exactly match the array of `InstanceData` structs we will upload in our draw function.
                .stepMode = WGPUVertexStepMode_Instance,
                .attributeCount = 2,
                .attributes = to_ptr<WGPUVertexAttribute>({
                    // Translation as a 3D vector.
                    {
                        .format = WGPUVertexFormat_Float32x3,
                        .offset = offsetof(InstanceData, translation),
                        .shaderLocation = 2
                    },
                    // Scale as a 2D vector for non-uniform scaling.
                    {
                        .format = WGPUVertexFormat_Float32x2,
                        .offset = offsetof(InstanceData, scale),
                        .shaderLocation = 3
                    }
                    })
            }
            })
        },
    
    // Interpret our 4 vertices as a triangle strip
    .primitive = WGPUPrimitiveState{
        .topology = WGPUPrimitiveTopology_TriangleStrip,
        },
    
    // No multi-sampling (1 sample per pixel, all bits on).
    .multisample = WGPUMultisampleState{
        .count = 1,
        .mask = ~0u
        },
    
    // Describe the fragment shader and its output
    .fragment = to_ptr( WGPUFragmentState{
        .module = shader_module,
        .entryPoint = "fragment_shader_main",
        
        // Our fragment shader outputs a single color value per pixel.
        .targetCount = 1,
        .targets = to_ptr<WGPUColorTargetState>({
            {
                .format = swap_chain_format,
                // The images we want to draw may have transparency, so let's turn on alpha blending with over compositing (ɑ⋅foreground + (1-ɑ)⋅background).
                // This will blend with whatever has already been drawn.
                .blend = to_ptr( WGPUBlendState{
                    // Over blending for color
                    .color = {
                        .operation = WGPUBlendOperation_Add,
                        .srcFactor = WGPUBlendFactor_SrcAlpha,
                        .dstFactor = WGPUBlendFactor_OneMinusSrcAlpha
                        },
                    // Leave destination alpha alone
                    .alpha = {
                        .operation = WGPUBlendOperation_Add,
                        .srcFactor = WGPUBlendFactor_Zero,
                        .dstFactor = WGPUBlendFactor_One
                        }
                    } ),
                .writeMask = WGPUColorWriteMask_All
            }})
        } )
    } ) );



   // wgpuQueueWriteBuffer( queue, uniform_buffer, 0, &uniforms, sizeof(Uniforms) );
}
bool GraphicsManager::LoadTexture(const std::string& name, const std::string& path )
{
    //printf("%s",name);
    int width, height, channels;
    unsigned char* data = stbi_load( path.c_str(), &width, &height, &channels, 4 );

    if (!data) {
        // Handle loading failure
        return false;
    }


    WGPUTexture tex = wgpuDeviceCreateTexture( device, to_ptr( WGPUTextureDescriptor{
    .usage = WGPUTextureUsage_TextureBinding | WGPUTextureUsage_CopyDst,
    .dimension = WGPUTextureDimension_2D,
    .size = { (uint32_t)width, (uint32_t)height, 1 },
    .format = WGPUTextureFormat_RGBA8UnormSrgb,
    .mipLevelCount = 1,
    .sampleCount = 1
    } ) );

    wgpuQueueWriteTexture(
    queue,
    to_ptr<WGPUImageCopyTexture>({ .texture = tex }),
    data,
    width * height * 4,
    to_ptr<WGPUTextureDataLayout>({ .bytesPerRow = (uint32_t)(width*4), .rowsPerImage = (uint32_t)height }),
    to_ptr( WGPUExtent3D{ (uint32_t)width, (uint32_t)height, 1 } )
    );

     image_map[name].texture = tex;
     image_map[name].width = width;
     image_map[name].height = height;
      stbi_image_free( data );


   
     return true;
}
void GraphicsManager::Draw()
{

    // Declare a vector to store sprites
    std::vector<Sprite> sprites;
    ECS.ForEach<Sprite>( [&]( EntityID entity ) {
        sprites.push_back( ECS.Get<Sprite>(entity) );
        // inside the loop
    } );
    
  
    

    if (sprites.empty()) {
        printf("skipped rendering");
        // Skip rendering if there are no sprites.
        
    }
    WGPUBuffer instance_buffer = wgpuDeviceCreateBuffer( device, to_ptr<WGPUBufferDescriptor>({
    .usage = WGPUBufferUsage_CopyDst | WGPUBufferUsage_Vertex,
    .size = sizeof(InstanceData) * sprites.size()
    }) );
    WGPUCommandEncoderRef encoder = wgpuDeviceCreateCommandEncoder( device, nullptr );
    WGPUTextureViewRef current_texture_view = wgpuSwapChainGetCurrentTextureView( swapchain );
    render_pass = wgpuCommandEncoderBeginRenderPass( encoder, to_ptr<WGPURenderPassDescriptor>({
    .colorAttachmentCount = 1,
    .colorAttachments = to_ptr<WGPURenderPassColorAttachment>({{
        .view = current_texture_view,
        .loadOp = WGPULoadOp_Clear,
        .storeOp = WGPUStoreOp_Store,
        // Choose the background color.
        .clearValue = WGPUColor{255, 83, 1, 0 }
        }})
    }) );

    wgpuRenderPassEncoderSetPipeline( render_pass, pipeline );
    wgpuRenderPassEncoderSetVertexBuffer( render_pass, 0 /* slot */, vertex_buffer, 0, 4*4*sizeof(float) );
    wgpuRenderPassEncoderSetVertexBuffer( render_pass, 1 /* slot */, instance_buffer, 0, sizeof(InstanceData) * sprites.size());

    auto spritescopy = sprites;

    
    // Start with an identity matrix.
    uniforms.projection = mat4{1};
    // Scale x and y by 1/100.
   
    uniforms.projection[0][0] = uniforms.projection[1][1] = 1./100.;
    // Scale the long edge by an additional 1/(long/short) = short/long.
   // int width, height;
    glfwGetFramebufferSize( window, &width, &height );
    if( width < height ) {
        uniforms.projection[1][1] *= width;
        uniforms.projection[1][1] /= height;
    } else {
        uniforms.projection[0][0] *= height;
        uniforms.projection[0][0] /= width;
    }
    wgpuQueueWriteBuffer( queue, uniform_buffer, 0, &uniforms, sizeof(Uniforms) );
    std::sort( spritescopy.begin(), spritescopy.end(), []( const Sprite& lhs, const Sprite& rhs ) { return lhs.z > rhs.z; } );
    
    for(int i = 0; i<sprites.size(); i++){
            Sprite& sprite = spritescopy[i];
            InstanceData instance;
            instance.scale = sprite.scale;  
            // Or initialize in the constructor directly 
            vec3 u = {sprite.position[0], sprite.position[1], sprite.z};
            instance.translation =   u;
            
            int image_width = image_map[sprite.image_name].width;
            int image_height = image_map[sprite.image_name].height ;
            

            //------------------WAS TOLD I NEED TO MULTIPLE THE VECTORS??? PLEASE HELP-----------
            if( image_width <image_height) {
                instance.scale = vec2(double(image_width)/image_height, 1.0 );
            } else {
                instance.scale = vec2( 1.0, double(image_height)/image_width );
            }

            instance.scale.x*= sprite.scale.x;
            instance.scale.y*= sprite.scale.y;

            wgpuQueueWriteBuffer( queue, instance_buffer, i * sizeof(InstanceData), &instance, sizeof(InstanceData) );

            auto layout = wgpuRenderPipelineGetBindGroupLayout( pipeline, 0 );
            WGPUBindGroup bind_group = wgpuDeviceCreateBindGroup( device, to_ptr( WGPUBindGroupDescriptor{
                .layout = layout,
                .entryCount = 3,
                // The entries `.binding` matches what we wrote in the shader.
                .entries = to_ptr<WGPUBindGroupEntry>({
                    {
                        .binding = 0,
                        .buffer = uniform_buffer,
                        .size = sizeof( Uniforms )
                    },
                    {
                        .binding = 1,
                        .sampler = textSampler,
                    },
                    {
                        .binding = 2,
                        .textureView = wgpuTextureCreateView( image_map[sprite.image_name].texture, nullptr )
                    }
                    })
                } ) );

                //------------IS THIS SUPPOSE TO GO IN TNE LOOP???--------
            wgpuBindGroupLayoutRelease( layout );
            wgpuRenderPassEncoderSetBindGroup( render_pass, 0, bind_group, 0, nullptr ); 
            wgpuRenderPassEncoderDraw( render_pass, 4, 1, 0, i );
         
            
    }
    
     guiManager.Draw(render_pass); //drawing imgui here

     wgpuRenderPassEncoderEnd(render_pass);       
     WGPUCommandBuffer command = wgpuCommandEncoderFinish( encoder, nullptr );
     wgpuQueueSubmit( queue, 1, &command );
     wgpuSwapChainPresent( swapchain );
     wgpuBufferRelease(instance_buffer);
     

}

json GraphicsManager::SerializeData()
{
    json spriteData;

    for (const auto& entitySpritePair : ECS.GetAppropriateSparseSet<GraphicsManager::Sprite>()) {
        const EntityID entity = entitySpritePair.first;
        const GraphicsManager::Sprite& sprite = entitySpritePair.second;

        json spriteInfo;
        spriteInfo["entity"] = entity;
        spriteInfo["image"] = sprite.image_name;
        spriteData.push_back(spriteInfo);
    }
    return spriteData;
}

void GraphicsManager::SaveSprites(const std::string &filename)
{
    json spriteData = SerializeData(); 
    std::ofstream file(filename);
    if (file.is_open()) {
        file << spriteData.dump(4); // Dump the JSON with indentation for readability
        file.close();
    }
}

void GraphicsManager::LoadSprite(const std::string &filename)
{
    std::ifstream file(filename);
    if (!file.is_open()) {
        std::cerr << "Failed to open sprite file: " << filename << std::endl;
        return;
    }

    // Parse the JSON data
    nlohmann::json spriteData;
    try {
        file >> spriteData;
    } catch (const nlohmann::json::exception& e) {
        std::cerr << "Error parsing JSON data: " << e.what() << std::endl;
        file.close();
        return;
    }

    file.close();
    //works, ignore error
    if (!spriteData.is_array()) {
        std::cerr << "Sprite data is not in the expected array format." << std::endl;
        return;
    }

    for (const auto& spriteInfo : spriteData) {
        if (spriteInfo.is_object()) {
            EntityID entity = spriteInfo["entity"];
            std::string image_name = spriteInfo["image"];

            // Create or update Sprite components in the EntityManager
            if (ECS.GetAppropriateSparseSet<GraphicsManager::Sprite>().count(entity) > 0) {
                ECS.Get<GraphicsManager::Sprite>(entity).image_name = image_name;
            }
        }
    }
} 
