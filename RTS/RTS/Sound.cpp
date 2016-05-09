#include"Sound.h"

Sound::Sound(std::string tag, SoundType soundType) {
	this->assetTag = tag;
	this->soundType = soundType;
}

Sound::Sound(const rapidjson::Value& root) {
	this->assetTag = root["assetTag"].GetString();
	std::string type = root["soundType"].GetString();
	if (type == "SOUND") {
		this->soundType = SoundType::SOUND;
	}
	else if (type == "MUSIC") {
		this->soundType = SoundType::MUSIC;
	}
	else {
		throw std::exception("Illegal sound type found.");
	}
}

void Sound::serialize(Serializer& serializer) const {
	serializer.writer.StartObject();

	serializer.writer.String("assetTag");
	serializer.writer.String(this->assetTag.c_str());

	if (this->soundType == SoundType::SOUND) {
		serializer.writer.String("soundType");
		serializer.writer.String("SOUND");
	}
	else if (this->soundType == SoundType::MUSIC) {
		serializer.writer.String("soundType");
		serializer.writer.String("MUSIC");
	}

	serializer.writer.EndObject();
}

SoundController::SoundController(SoundPtr sound) {
	this->mSound = sound;
}