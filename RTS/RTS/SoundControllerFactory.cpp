#include"SoundControllerFactory.h"

SoundController* SoundControllerFactory::createSound(std::string& assetTag, SoundType soundType) {
	SoundSystem* soundSystem = reinterpret_cast<SoundSystem*>(mSystemManager->systems.at(SystemType::SOUND));
	return soundSystem->createController(assetTag, soundType);
}