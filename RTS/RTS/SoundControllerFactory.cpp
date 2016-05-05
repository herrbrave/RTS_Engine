#include"SoundControllerFactory.h"

SoundController* SoundControllerFactory::createSound(std::string& assetTag, SoundType soundType) {
	SoundSystemPtr soundSystem = static_pointer_cast<SoundSystem>(mSystemManager->systems.at(SystemType::SOUND));
	return soundSystem->createController(assetTag, soundType);
}