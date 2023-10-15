#ifndef INPUTMANAGER_H
#define INPUTMANAGER_H
#include <GLFW/glfw3.h>

//class GraphicsManager; 

class InputManager{

        public:
                InputManager();
        // ~InputManager();

                void Start();
                void Shutdown();
                void Update();
                bool KeyisPressed(int key);
                bool KeyisReleased(int key);



        private:
              





};

#endif