#include"Ui.h"

ButtonConfig* createButtonConfig(const std::string& path, SystemManager* systemManager) {
	std::ifstream file;
	file.open(path);
	std::string builder;
	std::string line;
	while (std::getline(file, line)) {
		builder.append(line);
	}
	file.close();

	rapidjson::Document doc;
	doc.Parse(builder.c_str());

	const rapidjson::Value& parsedButton = doc["button"];
	std::string image = parsedButton["image"].GetString();
	systemManager->graphicsSystem->addTexture(image, image);

	ButtonConfig* buttonConfig = new ButtonConfig();
	buttonConfig->sectionWidth = parsedButton["section_width"].GetInt();
	buttonConfig->sectionHeight = parsedButton["section_height"].GetInt();

	parseButtonState(image, parsedButton["button_up"], buttonConfig->buttonUp);
	parseButtonState(image, parsedButton["button_over"], buttonConfig->buttonOver);
	parseButtonState(image, parsedButton["button_down"], buttonConfig->buttonDown);
	
	return buttonConfig;
}

void parseButtonState(const std::string& tag, const rapidjson::Value& button, std::unordered_map<std::string, Texture*>& stateMap) {
	for (auto it = button.MemberBegin(); it != button.MemberEnd(); it++) {
		std::string name = it->name.GetString();
		const rapidjson::Value& value = it->value;
		Texture* texture = new Texture(tag, value["x"].GetDouble(), value["y"].GetDouble(), value["w"].GetDouble(), value["h"].GetDouble());
		stateMap.emplace(name, texture);
	}
}


ButtonSectionDrawable::ButtonSectionDrawable(float width, float height, vector2f* positionOffset, Texture* texture) : Drawable(width, height) {
	mPosOffset.reset(positionOffset);
	mTexture = texture;
}

void ButtonSectionDrawable::draw(Graphics* graphicsRef, const vector2f* position) {
	vector2f translated = *position + *mPosOffset.get();
	graphicsRef->renderTexture(mTexture, translated.x, translated.y, width, height, mColor->r, mColor->g, mColor->b, mColor->a);
}

ButtonDrawable::ButtonDrawable(float width, float height, const ButtonConfig& buttonConfig) : Drawable(width, height) {
	state = ButtonState::UP;
	mSections[ButtonState::UP] = std::vector<ButtonSectionDrawable*>();
	mSections[ButtonState::OVER] = std::vector<ButtonSectionDrawable*>();
	mSections[ButtonState::DOWN] = std::vector<ButtonSectionDrawable*>();

	float sectionWidth = buttonConfig.sectionWidth;
	float sectionHeight = buttonConfig.sectionHeight;
	float halfWidth = width / 2;
	float halfHeight = height / 2;

	int dirs[] = { -1, 0, 1 };
	std::string names[9] = { "upper_left", "upper_middle", "upper_right", "middle_left", "middle", "middle_right", "bottom_left", "bottom_middle", "bottom_right" };
	float size[9][2] = {
		{ sectionWidth, sectionHeight },
		{ width - (2 * sectionWidth), sectionHeight },
		{ sectionWidth, sectionHeight },
		{ sectionWidth, height -(2 * sectionHeight) },
		{ width - (2 * sectionWidth), height - (2 * sectionHeight) },
		{ sectionWidth, height - (2 * sectionHeight) },
		{ sectionWidth, sectionHeight },
		{ width - (2 * sectionWidth), sectionHeight },
		{ sectionWidth, sectionHeight }
	};

	int count = 0;
	for (int yDir : dirs) {
		for (int xDir : dirs) {
			vector2f* posOffset = new vector2f((halfWidth - sectionWidth) * xDir, (halfHeight - sectionHeight) * yDir);
			std::string name = names[count];

			ButtonSectionDrawable* upDrawable = new ButtonSectionDrawable(size[count][0], size[count][1], posOffset, buttonConfig.buttonUp.at(name));
			ButtonSectionDrawable* overDrawable = new ButtonSectionDrawable(size[count][0], size[count][1], posOffset, buttonConfig.buttonOver.at(name));
			ButtonSectionDrawable* downDrawable = new ButtonSectionDrawable(size[count][0], size[count][1], posOffset, buttonConfig.buttonDown.at(name));
			mSections.at(ButtonState::UP).push_back(upDrawable);
			mSections.at(ButtonState::OVER).push_back(overDrawable);
			mSections.at(ButtonState::DOWN).push_back(downDrawable);
			count++;
		}
	}
}

void ButtonDrawable::draw(Graphics* graphicsRef, const vector2f* position) {
	for (auto drawable : mSections[state]) {
		drawable->draw(graphicsRef, position);
	}
}

void ButtonComponent::onMouseEvent(const MouseEvent& mouseEvent, SystemManager* systemManager) {
	if (mouseEvent.action == MouseAction::MOVE) {
		Body* body = systemManager->physicsSystem->getBody(entityId);
		if (body->checkPoint(*mouseEvent.position)) {
			ButtonDrawable* drawable = reinterpret_cast<ButtonDrawable*>(systemManager->graphicsSystem->getDrawableById(entityId));
			drawable->state = ButtonState::OVER;
		}
		else {
			ButtonDrawable* drawable = reinterpret_cast<ButtonDrawable*>(systemManager->graphicsSystem->getDrawableById(entityId));
			drawable->state = ButtonState::UP;
		}
	}
	else if (mouseEvent.action == MouseAction::CLICK_DOWN) {
		ButtonDrawable* drawable = reinterpret_cast<ButtonDrawable*>(systemManager->graphicsSystem->getDrawableById(entityId));
		if (drawable->state == ButtonState::OVER) {
			drawable->state = ButtonState::DOWN;
		}
	}
	else {
		ButtonDrawable* drawable = reinterpret_cast<ButtonDrawable*>(systemManager->graphicsSystem->getDrawableById(entityId));
		if (drawable->state == ButtonState::DOWN) {
			drawable->state = ButtonState::OVER;
			mCallback();
		}
	}
}

void ButtonComponent::setCallback(const std::function<void()>& callback) {
	mCallback = std::move(callback);
}


WidgetFactory::WidgetFactory(std::string buttonConfigPath, SystemManager* systemManager) : EntityFactory(systemManager) {
	mButtonConfig.reset(createButtonConfig(buttonConfigPath, systemManager));
}

Entity* WidgetFactory::createButton(std::function<void()> callback, float x, float y, float width, float height) {
	Entity* entity = new Entity();
	mSystemManager->entitySystem->registerEntity(entity);

	Body* blockBody = new BlockBody(x, y, width, height);
	mSystemManager->physicsSystem->registerBody(entity->id, blockBody);
	PhysicsComponent* physicsComponent = new PhysicsComponent(entity->id, blockBody);

	Drawable* textureDrawable = new ButtonDrawable(width, height, *mButtonConfig.get());
	mSystemManager->graphicsSystem->registerDrawable(entity->id, textureDrawable);
	DrawableComponent* drawableComponent = new DrawableComponent(entity->id, textureDrawable);

	ButtonComponent* buttonComponent = new ButtonComponent(entity->id);
	buttonComponent->setCallback(callback);

	entity->componentContainer->registerComponent(physicsComponent);
	entity->componentContainer->registerComponent(drawableComponent);
	entity->componentContainer->registerComponent(buttonComponent);

	return entity;
}