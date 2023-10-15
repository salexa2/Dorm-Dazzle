#ifndef RESOURCEMANAGER_H
#define RESOURCEMANAGER_H
#include <filesystem>
#include <string>

class ResourceManager{

    private:
      //  std::filesystem::path rootPath;
    public: //= "assets"
         // Method to resolve a partial path to a real file path
        ResourceManager();
        std::string passString(std::string path);
        void Start();
        void Shutdown();



};

#endif