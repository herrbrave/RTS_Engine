#ifndef __SOUND_CONTROLLER_H__
#define __SOUND_CONTROLLER_H__

#include"Sound.h"
#include"System.h"

class SoundControllerFactory;
typedef shared_ptr<SoundControllerFactory> SoundControllerFactoryPtr;
typedef weak_ptr<SoundControllerFactory> WeakSoundControllerFactoryPtr;

class SoundControllerFactory {
public:
	SoundControllerFactory(SystemManagerPtr systemManager) {
		mSystemManager = systemManager;
	}

	SoundControllerPtr createSound(std::string& assetTag, SoundType soundType);

private:
	SystemManagerPtr mSystemManager;
};

#endif // !__SOUND_CONTROLLER_H__
