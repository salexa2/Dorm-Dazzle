#include "SoundManager.h"

void SoundManager::StartUp() {
	soloud.init();
}

void SoundManager::ShutDown() {
	soloud.deinit();
}


bool SoundManager::LoadSound(const std::string& name, const std::string& path) {

	sound_map[name].load(path.c_str());

	return true; //default return
}

bool SoundManager::PlaySound(const std::string& name) {

	if (sound_map.count(name) == 0) {
		printf("Sound is not loaded.");
		return false;
	}

	soloud.play(sound_map[name]);

	return true;
}

bool SoundManager::PlaySoundLooping(const std::string& name){
	if(sound_map.count(name) == 0){
		printf("Sound is not loaded.");
		return false;
	}

	// SoLoud::Wav wav = ; //get the wav file to loop

	// sound_map[name].setLoopPoint(sound_map[name].getLength()); //set the loop point...
	sound_map[name].setLooping(1);
	soloud.play(sound_map[name]); //play the sound...



	// //set looping
	// soloud.setLooping(sound_map[name], 1); //not sure what the 1 is for?
	// //play sound
	// soloud.play(sound_map[name]);
	return true;
}

bool SoundManager::DeleteSound(const std::string& name) {
	sound_map.erase(name);

	if (sound_map.count(name) > 0) {
		printf("Sound was not properly deleted.");
		return false;
	}

	return true;
}