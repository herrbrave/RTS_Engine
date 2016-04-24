#ifndef __SOUND_H__
#define __SOUND_H__

#include"Asset.h"
#include"Serializer.h"

#include<rapidjson\document.h>
#include<memory>
#include<string>

enum class SoundType {
	SOUND = 0,
	MUSIC = 1,
};

class Sound {
public:
	std::string assetTag;
	SoundType soundType;

	Sound(std::string tag, SoundType soundType);

	Sound(const rapidjson::Value& root);

	void serialize(Serializer& serializer) const;
};

class SoundController {
public:
	SoundController(Sound* sound);

	virtual void play(int loop = 0) = 0;
	virtual void pause() = 0;
	virtual void stop() = 0;
protected:
	std::unique_ptr<Sound> mSound;
};

#endif // !__SOUND_H__
