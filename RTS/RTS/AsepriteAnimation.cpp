#include"AsepriteAnimation.h"



void parseFrames(const rapidjson::Value& value, AsepriteFrames& frames) {
	auto it = value.Begin();
	while (it != value.End()) {
		const rapidjson::Value& val = (*it);
		AsepriteFramePtr frame = std::make_shared<AsepriteFrame>();

		frame->x = val["frame"]["x"].GetInt();
		frame->y = val["frame"]["y"].GetInt();
		frame->w = val["frame"]["w"].GetInt();
		frame->h = val["frame"]["h"].GetInt();
		frame->sourceW = val["sourceSize"]["w"].GetInt();
		frame->sourceH = val["sourceSize"]["h"].GetInt();
		frame->duration = val["duration"].GetInt();
		frame->rotated = val["rotated"].GetBool();
		frame->trimmed = val["trimmed"].GetBool();

		frames.push_back(frame);

		it++;
	}
}

AsepriteMetadataPtr parseMetadata(const rapidjson::Value& value) {
	AsepriteMetadataPtr metadata = std::make_shared<AsepriteMetadata>();

	metadata->w = value["size"]["w"].GetInt();
	metadata->h = value["size"]["h"].GetInt();

	// parse tags
	parseTags(value["frameTags"], metadata->frameTags);

	return metadata;
}

void parseTags(const rapidjson::Value& value, AsepriteTags& tags) {
	auto it = value.Begin();
	while (it != value.End()) {
		const rapidjson::Value& val = (*it);
		auto tag = std::make_shared<AsepriteTag>();
		tag->name = val["name"].GetString();
		tag->from = val["from"].GetInt();
		tag->to = val["to"].GetInt();
		tag->direction = val["direction"].GetString();
		tags.push_back(tag);
		it++;
	}
}

AsepriteAnimationPtr parseAnimation(const string& path) {
	std::ifstream file;
	file.open(path);
	if (!file.is_open()) {
		ERR("Unable to open file: " + path);
	}
	std::string line;
	std::string builder;
	while (std::getline(file, line)) {
		builder.append(line);
	}
	file.close();

	rapidjson::Document doc;
	doc.Parse(builder.c_str());

	AsepriteAnimationPtr animation = std::make_shared<AsepriteAnimation>();

	// parse frames
	parseFrames(doc["frames"], animation->frames);

	// parse metadata
	animation->meta = parseMetadata(doc["meta"]);

	return animation;
}