#ifndef __SOUND_CONTROLLER_H__
#define __SOUND_CONTROLLER_H__

#include"Sound.h"
#include"System.h"

class SoundControllerFactory {
public:
	SoundControllerFactory(SystemManager* systemManager) {
		mSystemManager = systemManager;
	}

	SoundController* createSound(std::string& assetTag, SoundType soundType);

private:
	SystemManager* mSystemManager;
};

#endif // !__SOUND_CONTROLLER_H__
