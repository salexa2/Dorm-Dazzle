//#define GLFW_INCLUDE_NONE
#include <iostream>
#include "InputManager.h"
#include "Engine.h"

InputManager::InputManager(){

}

void InputManager::Start(){
    
}
void Shutdown(){
    
}

void InputManager::Update(){
    glfwPollEvents();
}

bool InputManager::KeyisPressed(int key){
   
    if(glfwGetKey(GLOBAL_ENGINE.graphicsManager.window, key)== GLFW_PRESS){ 
        printf("Key pressed\n");
        return true;
     } 
    return false;
}

bool InputManager::KeyisReleased(int key)
{
    if(glfwGetKey(GLOBAL_ENGINE.graphicsManager.window, key)== GLFW_RELEASE){
        return true;
     } 
    return false;
   
}
