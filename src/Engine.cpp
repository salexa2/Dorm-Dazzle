
#include "Engine.h"
#include <thread>
#include <chrono>

#include <iostream> //for printing (JM)
#include <fstream>
#include <ctime>

using namespace std;

// Constructor
Engine::Engine()
{
}

// Other managers

// Methods
void Engine::Start()
{

    graphicsManager.Start();
    scriptManager.Start();

    soundManager.StartUp(); // JM
}

void Engine::Shutdown()
{

    SaveTime();

    soundManager.ShutDown(); // JM

    graphicsManager.Shutdown();
    // inputManager.Shutdown();
}

void Engine::SaveTime()
{
    // save time of shutdown to file

    // write to file for storage
    std::ofstream timeFile("time.txt");

    if (timeFile.is_open())
    {
        auto timenow = chrono::system_clock::to_time_t(chrono::system_clock::now());
        timeFile << std::ctime(&timenow);
        timeFile.close();
        std::cout << "Current time has been written to time.txt" << std::endl;
    }
    else
    {
        std::cerr << "Unable to open file to write current time." << std::endl;
    }
}

void Engine::ExitGame()
{
    glfwSetWindowShouldClose(GLOBAL_ENGINE.graphicsManager.window, true);
}

//----------HELP HELP HELP
void Engine::RunGameLoop(const UpdateCallBack &callback)
{

    // gui progress bar initialization
    GLOBAL_ENGINE.graphicsManager.guiManager.InitProgressBar();

    while (true)
    {
        inputManager.Update();
        UpdateCallBack();
        //
        callback(*this);
        if (glfwWindowShouldClose(graphicsManager.window) == true)
        {
            break;
        }
        graphicsManager.Draw();
        //  GLOBAL_ENGINE.graphicsManager.guiManager.Draw(GLOBAL_ENGINE.graphicsManager.render_pass);

        const auto t1 = std::chrono::steady_clock::now();
        const auto t2 = std::chrono::steady_clock::now();
        const auto sub = t1 - t2;
        const auto sixty = std::chrono::duration<double>(sub);
        std::this_thread::sleep_for(sixty);
    }
}

// Engine GLOBAL_ENGINE;

// Manage timestep
