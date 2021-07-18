#pragma once

#include"Constants.h"

#include<fstream>
#include<iostream>

#include<rapidjson\document.h>

class AsepriteFrame;
typedef shared_ptr<AsepriteFrame> AsepriteFramePtr;
typedef weak_ptr<AsepriteFrame> WeakAsepriteFramePtr;
typedef vector<AsepriteFramePtr> AsepriteFrames;

class AsepriteMetadata;
typedef shared_ptr<AsepriteMetadata> AsepriteMetadataPtr;
typedef weak_ptr<AsepriteMetadata> WeakAsepriteMetadataPtr;

class AsepriteTag;
typedef shared_ptr<AsepriteTag> AsepriteTagPtr;
typedef weak_ptr<AsepriteTag> WeakAsepriteTagPtr;
typedef vector<AsepriteTagPtr> AsepriteTags;

class AsepriteAnimation;
typedef shared_ptr<AsepriteAnimation> AsepriteAnimationPtr;
typedef weak_ptr<AsepriteAnimation> WeakAsepriteAnimationPtr;

class AsepriteFrame {
public:
	int x;
	int y;
	int w;
	int h;
	int sourceW;
	int sourceH;
	bool rotated;
	bool trimmed;
	int duration;
};

class AsepriteMetadata {
public:
	int w;
	int h;
	AsepriteTags frameTags;
};

class AsepriteTag {
public:
	string name;
	int from;
	int to;
	string direction;
};

class AsepriteAnimation {
public:
	AsepriteFrames frames;
	AsepriteMetadataPtr meta;
};

#ifndef __ASEPRITE_ANIMATION_PARSER__
#define __ASEPRITE_ANIMATION_PARSER__

void parseFrames(const rapidjson::Value& value, AsepriteFrames& frames);
AsepriteMetadataPtr parseMetadata(const rapidjson::Value& value);
void parseTags(const rapidjson::Value& value, AsepriteTags& tags);
AsepriteAnimationPtr parseAnimation(const string& path);

#endif // !__ASEPRITE_ANIMATION_PARSER__