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

	parseState(image, parsedButton["button_up"], buttonConfig->buttonUp);
	parseState(image, parsedButton["button_over"], buttonConfig->buttonOver);
	parseState(image, parsedButton["button_down"], buttonConfig->buttonDown);
	
	return buttonConfig;
}

PanelConfig* createPanelConfig(const std::string& path, SystemManager* systemManager) {
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

	const rapidjson::Value& parsedPanel = doc["panel"];
	std::string image = parsedPanel["image"].GetString();
	systemManager->graphicsSystem->addTexture(image, image);

	PanelConfig* panelConfig = new PanelConfig();
	panelConfig->sectionWidth = parsedPanel["section_width"].GetInt();
	panelConfig->sectionHeight = parsedPanel["section_height"].GetInt();

	parseState(image, parsedPanel["sections"], panelConfig->panelSections);

	return panelConfig;
}

void parseState(const std::string& tag, const rapidjson::Value& button, std::unordered_map<std::string, Texture*>& stateMap) {
	for (auto it = button.MemberBegin(); it != button.MemberEnd(); it++) {
		std::string name = it->name.GetString();
		const rapidjson::Value& value = it->value;
		Texture* texture = new Texture(tag, value["x"].GetDouble(), value["y"].GetDouble(), value["w"].GetDouble(), value["h"].GetDouble());
		stateMap.emplace(name, texture);
	}
}


SectionDrawable::SectionDrawable(float width, float height, vector2f* positionOffset, Texture* texture) : Drawable(width, height) {
	mPosOffset.reset(positionOffset);
	mTexture = texture;
}

void SectionDrawable::draw(Graphics* graphicsRef, const vector2f* position) {
	vector2f translated = *position + *mPosOffset.get();
	graphicsRef->renderTexture(mTexture, translated.x, translated.y, width, height, mColor->r, mColor->g, mColor->b, mColor->a);
}

ButtonDrawable::ButtonDrawable(float width, float height, const ButtonConfig& buttonConfig) : Drawable(width, height) {
	state = ButtonState::UP;
	mSections[ButtonState::UP] = std::vector<SectionDrawable*>();
	mSections[ButtonState::OVER] = std::vector<SectionDrawable*>();
	mSections[ButtonState::DOWN] = std::vector<SectionDrawable*>();

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
			vector2f* posOffset = new vector2f((halfWidth - (sectionWidth / 2)) * xDir, (halfHeight - (sectionHeight / 2)) * yDir);
			std::string name = names[count];

			SectionDrawable* upDrawable = new SectionDrawable(size[count][0], size[count][1], posOffset, buttonConfig.buttonUp.at(name));
			SectionDrawable* overDrawable = new SectionDrawable(size[count][0], size[count][1], posOffset, buttonConfig.buttonOver.at(name));
			SectionDrawable* downDrawable = new SectionDrawable(size[count][0], size[count][1], posOffset, buttonConfig.buttonDown.at(name));
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

	if (mButtonTexture != nullptr) {
		graphicsRef->renderTexture(mButtonTexture.get(), position->x, position->y, -1, -1, 255, 255, 255, 255);
	}
}


void ButtonDrawable::setButtonTexture(Texture* texture) {
	mButtonTexture.reset(texture);
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

void ButtonComponent::setText(const std::string& text, SystemManager* systemManager) {
	systemManager->graphicsSystem->createTextSurface(text, text, 0, 0, 0, 255);
	Texture* textTexture = new Texture(text);
	mText = text;

	ButtonDrawable* drawable = reinterpret_cast<ButtonDrawable*>(systemManager->graphicsSystem->getDrawableById(entityId));
	drawable->setButtonTexture(textTexture);
}

std::string& ButtonComponent::getText() {
	return mText;
}

void ButtonComponent::setIcon(Texture* texture, SystemManager* systemManager) {
	ButtonDrawable* drawable = reinterpret_cast<ButtonDrawable*>(systemManager->graphicsSystem->getDrawableById(entityId));
	drawable->setButtonTexture(texture);
}

void setButtonText(Entity* entity, const std::string& text, SystemManager* systemManager) {
	ButtonComponent* buttonComponent = reinterpret_cast<ButtonComponent*>(entity->componentContainer->getComponentByType(ComponentType::BUTTON_COMPONENT));
	buttonComponent->setText(text, systemManager);
}

void setIcon(Entity* entity, const std::string& assetTag, float tx, float ty, float tw, float th, SystemManager* systemManager) {
	Texture* texture = new Texture(assetTag, tx, ty, tw, th);

	ButtonComponent* buttonComponent = reinterpret_cast<ButtonComponent*>(entity->componentContainer->getComponentByType(ComponentType::BUTTON_COMPONENT));
	buttonComponent->setIcon(texture, systemManager);
}

PanelDrawable::PanelDrawable(float width, float height, const PanelConfig& panelConfig) : Drawable(width, height) {
	float sectionWidth = panelConfig.sectionWidth;
	float sectionHeight = panelConfig.sectionHeight;
	float doubleWidth = (sectionWidth * 2);
	float doubleHeight = (sectionHeight * 2);
	float halfWidth = width / 2;
	float halfHeight = height / 2;

	int dirs[] = { -1, 0, 1 };
	std::string names[9] = { "upper_left", "upper_middle", "upper_right", "middle_left", "middle", "middle_right", "bottom_left", "bottom_middle", "bottom_right" };
	float size[9][2] = {
		{ sectionWidth, sectionHeight },
		{ width - doubleWidth, sectionHeight },
		{ sectionWidth, sectionHeight },
		{ sectionWidth, height - doubleHeight },
		{ width - doubleWidth, height - doubleHeight },
		{ sectionWidth, height - doubleHeight },
		{ sectionWidth, sectionHeight },
		{ width - doubleWidth, sectionHeight },
		{ sectionWidth, sectionHeight }
	};

	int count = 0;
	for (int yDir : dirs) {
		for (int xDir : dirs) {
			vector2f* posOffset = new vector2f((halfWidth - (sectionWidth / 2)) * xDir, (halfHeight -(sectionHeight / 2)) * yDir);
			std::string name = names[count];

			SectionDrawable* sectionDrawable = new SectionDrawable(size[count][0], size[count][1], posOffset, panelConfig.panelSections.at(name));
			mSections.push_back(sectionDrawable);
			count++;
		}
	}
}

void PanelDrawable::draw(Graphics* graphicsRef, const vector2f* position) {
	for (auto drawable : mSections) {
		drawable->draw(graphicsRef, position);
	}
}

WidgetFactory::WidgetFactory(std::string buttonConfigPath, std::string panelConfigPath, SystemManager* systemManager) : EntityFactory(systemManager) {
	mButtonConfig.reset(createButtonConfig(buttonConfigPath, systemManager));
	mPanelConfig.reset(createPanelConfig(panelConfigPath, systemManager));
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

Entity* WidgetFactory::createPanel(float x, float y, float width, float height) {
	Entity* entity = new Entity();
	mSystemManager->entitySystem->registerEntity(entity);

	Body* blockBody = new BlockBody(x, y, width, height);
	mSystemManager->physicsSystem->registerBody(entity->id, blockBody);
	PhysicsComponent* physicsComponent = new PhysicsComponent(entity->id, blockBody);

	Drawable* textureDrawable = new PanelDrawable(width, height, *mPanelConfig.get());
	mSystemManager->graphicsSystem->registerDrawable(entity->id, textureDrawable);
	DrawableComponent* drawableComponent = new DrawableComponent(entity->id, textureDrawable);

	entity->componentContainer->registerComponent(physicsComponent);
	entity->componentContainer->registerComponent(drawableComponent);

	return entity;
}