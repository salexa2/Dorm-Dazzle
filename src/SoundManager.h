#ifndef SOUNDMANAGER_H
#define SOUNDMANAGER_H

#include <string>
#include <unordered_map>

#include "soloud.h"
#include "soloud_wav.h"

class SoundManager {
public:

	SoLoud::Soloud soloud; // Engine core
	//SoLoud::Wav sample;    // One sample

	std::unordered_map<std::string, SoLoud::Wav> sound_map;

	void StartUp();
	void ShutDown();

	bool LoadSound(const std::string& name, const std::string& path);
	bool PlaySound(const std::string& name);
	bool DeleteSound(const std::string& name);
};

#endif