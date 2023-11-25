#ifndef ENGINE_H
#define ENGINE_H
#include "GraphicsManager.h"
#include "InputManager.h"
#include "ResourceManager.h"
#include "EntityManager.h"
#include "ScriptManager.h"
#include "SoundManager.h"
#include <functional>



//Engine GLOBAL_ENGINE;

class Engine{

      
public:
    Engine();
    //   ~Engine();
    GraphicsManager graphicsManager;
    InputManager inputManager;
    ResourceManager resouceManager;
    ScriptManager scriptManager;
    SoundManager soundManager; //JM
            
           
    //start up managers
    void Start();
    //runs game 
    typedef std::function<void(Engine&)> UpdateCallBack;
    void RunGameLoop(const UpdateCallBack& callback);
    //shuts down managers
    void Shutdown();
    void ExitGame();

    void ExitToMenu();

    void SetPlayMode(bool mode);

};

 inline Engine GLOBAL_ENGINE;



#endif // ENGINE_H
