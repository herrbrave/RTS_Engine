#include"Ui.h"

ButtonConfigPtr createButtonConfig(const std::string& path, SystemManagerPtr systemManager) {
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
	GraphicsSystemPtr graphicsSystem = static_pointer_cast<GraphicsSystem>(systemManager->systems.at(SystemType::GRAPHICS));
	graphicsSystem->addTexture(image, image);

	ButtonConfig* buttonConfig = new ButtonConfig();
	buttonConfig->sectionWidth = parsedButton["section_width"].GetInt();
	buttonConfig->sectionHeight = parsedButton["section_height"].GetInt();

	parseState(image, parsedButton["button_up"], buttonConfig->buttonUp);
	parseState(image, parsedButton["button_over"], buttonConfig->buttonOver);
	parseState(image, parsedButton["button_down"], buttonConfig->buttonDown);
	
	return ButtonConfigPtr(buttonConfig);
}

PanelConfigPtr createPanelConfig(const std::string& path, SystemManagerPtr systemManager) {
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
	GraphicsSystemPtr graphicsSystem = static_pointer_cast<GraphicsSystem>(systemManager->systems.at(SystemType::GRAPHICS));
	graphicsSystem->addTexture(image, image);

	PanelConfig* panelConfig = new PanelConfig();
	panelConfig->sectionWidth = parsedPanel["section_width"].GetInt();
	panelConfig->sectionHeight = parsedPanel["section_height"].GetInt();

	parseState(image, parsedPanel["sections"], panelConfig->panelSections);

	return PanelConfigPtr(panelConfig);
}

void parseState(const std::string& tag, const rapidjson::Value& button, std::unordered_map<std::string, TexturePtr>& stateMap) {
	for (auto it = button.MemberBegin(); it != button.MemberEnd(); it++) {
		std::string name = it->name.GetString();
		const rapidjson::Value& value = it->value;
		TexturePtr texture(GCC_NEW Texture(tag, value["x"].GetDouble(), value["y"].GetDouble(), value["w"].GetDouble(), value["h"].GetDouble()));
		stateMap.emplace(name, texture);
	}
}


SectionDrawable::SectionDrawable(float width, float height, const Vector2f& positionOffset, TexturePtr texture) : Drawable(width, height) {
	mPosOffset = Vector2fPtr(const_cast<Vector2f*>(&positionOffset));
	mTexture = texture;
}

void SectionDrawable::draw(Graphics& graphicsRef, const Vector2f& position) {
	Vector2fPtr translated(&(position + *mPosOffset));
	graphicsRef.renderTexture(mTexture, translated->x, translated->y, width, height, mColor->r, mColor->g, mColor->b, mColor->a);
}

ButtonDrawable::ButtonDrawable(float width, float height, const ButtonConfig& buttonConfig) : Drawable(width, height) {
	isUi = true;
	state = ButtonState::UP;
	mSections[ButtonState::UP] = vector<SectionDrawablePtr>();
	mSections[ButtonState::OVER] = vector<SectionDrawablePtr>();
	mSections[ButtonState::DOWN] = vector<SectionDrawablePtr>();

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
			Vector2fPtr posOffset(GCC_NEW Vector2f((halfWidth - (sectionWidth / 2)) * xDir, (halfHeight - (sectionHeight / 2)) * yDir));
			std::string name = names[count];

			SectionDrawablePtr upDrawable(GCC_NEW SectionDrawable(size[count][0], size[count][1], posOffset, buttonConfig.buttonUp.at(name)));
			SectionDrawablePtr overDrawable(GCC_NEW SectionDrawable(size[count][0], size[count][1], posOffset, buttonConfig.buttonOver.at(name)));
			SectionDrawablePtr downDrawable(GCC_NEW SectionDrawable(size[count][0], size[count][1], posOffset, buttonConfig.buttonDown.at(name)));
			mSections.at(ButtonState::UP).push_back(upDrawable);
			mSections.at(ButtonState::OVER).push_back(overDrawable);
			mSections.at(ButtonState::DOWN).push_back(downDrawable);
			count++;
		}
	}
}

void ButtonDrawable::draw(Graphics& graphicsRef, const Vector2f& position) {
	for (auto drawable : mSections[state]) {
		drawable->draw(graphicsRef, position);
	}

	if (mButtonTexture != nullptr) {
		graphicsRef.renderTexture(mButtonTexture, position.x, position.y, -1, -1, 255, 255, 255, 255);
	}
}


void ButtonDrawable::setButtonTexture(TexturePtr texture) {
	mButtonTexture = texture;
}

void ButtonComponent::onMouseEvent(const MouseEvent& mouseEvent, SystemManagerPtr systemManager) {
	PhysicsSystemPtr physicsSystem = makeShared(systemManager->getSystemByType<PhysicsSystem>(SystemType::PHYSICS));
	GraphicsSystemPtr graphicsSystem = makeShared(systemManager->getSystemByType<GraphicsSystem>(SystemType::GRAPHICS));
	if (mouseEvent.action == MouseAction::MOVE) {
		BodyPtr body = makeShared(physicsSystem->getBody(entityId));
		if (body->checkPoint(*mouseEvent.position)) {
			ButtonDrawablePtr drawable = static_pointer_cast<ButtonDrawable>(makeShared(graphicsSystem->getDrawableById(entityId)));
			drawable->state = ButtonState::OVER;
		}
		else {
			ButtonDrawablePtr drawable = static_pointer_cast<ButtonDrawable>(makeShared(graphicsSystem->getDrawableById(entityId)));
			drawable->state = ButtonState::UP;
		}
	}
	else if (mouseEvent.action == MouseAction::CLICK_DOWN) {
		ButtonDrawablePtr drawable = static_pointer_cast<ButtonDrawable>(makeShared(graphicsSystem->getDrawableById(entityId)));
		if (drawable->state == ButtonState::OVER) {
			drawable->state = ButtonState::DOWN;
		}
	}
	else {
		ButtonDrawablePtr drawable = static_pointer_cast<ButtonDrawable>(makeShared(graphicsSystem->getDrawableById(entityId)));
		if (drawable->state == ButtonState::DOWN) {
			drawable->state = ButtonState::OVER;
			mCallback();
		}
	}
}

void ButtonComponent::setCallback(const std::function<void()>& callback) {
	mCallback = std::move(callback);
}

void ButtonComponent::setText(const std::string& text, SystemManagerPtr systemManager) {
	GraphicsSystemPtr graphicsSystem = static_pointer_cast<GraphicsSystem>(systemManager->systems.at(SystemType::GRAPHICS));
	graphicsSystem->createTextSurface(text, text, 0, 0, 0, 255);
	TexturePtr textTexture(GCC_NEW Texture(text));
	mText = text;

	ButtonDrawablePtr drawable = static_pointer_cast<ButtonDrawable>(makeShared(graphicsSystem->getDrawableById(entityId)));
	drawable->setButtonTexture(textTexture);
}

string& ButtonComponent::getText() {
	return mText;
}

void ButtonComponent::setIcon(TexturePtr texture, SystemManagerPtr systemManager) {
	GraphicsSystemPtr graphicsSystem = makeShared(systemManager->getSystemByType<GraphicsSystem>(SystemType::GRAPHICS));
	ButtonDrawablePtr drawable = static_pointer_cast<ButtonDrawable>(makeShared(graphicsSystem->getDrawableById(entityId)));
	drawable->setButtonTexture(texture);
}

void setButtonText(EntityPtr entity, const std::string& text, SystemManagerPtr systemManager) {
	weak_ptr<ButtonComponent> buttonComponent = entity->getComponentByType<ButtonComponent>(ComponentType::BUTTON_COMPONENT);
	if (auto ptr = buttonComponent.lock()) {
		ptr->setText(text, systemManager);
	}
}

void setIcon(EntityPtr entity, const std::string& assetTag, float tx, float ty, float tw, float th, SystemManagerPtr systemManager) {
	TexturePtr texture(GCC_NEW Texture(assetTag, tx, ty, tw, th));

	weak_ptr<ButtonComponent> buttonComponent = entity->getComponentByType<ButtonComponent>(ComponentType::BUTTON_COMPONENT);
	if (auto ptr = buttonComponent.lock()) {
		ptr->setIcon(texture, systemManager);
	}
}

PanelDrawable::PanelDrawable(float width, float height, const PanelConfig& panelConfig) : Drawable(width, height) {
	isUi = true;
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
			Vector2fPtr posOffset(GCC_NEW Vector2f((halfWidth - (sectionWidth / 2)) * xDir, (halfHeight -(sectionHeight / 2)) * yDir));
			std::string name = names[count];

			SectionDrawablePtr sectionDrawable(GCC_NEW SectionDrawable(size[count][0], size[count][1], posOffset, panelConfig.panelSections.at(name)));
			mSections.push_back(sectionDrawable);
			count++;
		}
	}
}

void PanelDrawable::draw(Graphics& graphicsRef, const Vector2f& position) {
	for (auto drawable : mSections) {
		drawable->draw(graphicsRef, position);
	}
}

WidgetFactory::WidgetFactory(std::string buttonConfigPath, std::string panelConfigPath, SystemManagerPtr systemManager) : EntityFactory(systemManager) {
	mButtonConfig = createButtonConfig(buttonConfigPath, systemManager);
	mPanelConfig = createPanelConfig(panelConfigPath, systemManager);
}

EntityPtr WidgetFactory::createButton(std::function<void()> callback, float x, float y, float width, float height) {
	EntitySystemPtr entitySystem = makeShared(mSystemManager->getSystemByType<EntitySystem>(SystemType::ENTITY));
	EntityPtr entity(GCC_NEW Entity());
	entitySystem->addEntity(entity);

	PhysicsSystemPtr physicsSystem = makeShared(mSystemManager->getSystemByType<PhysicsSystem>(SystemType::PHYSICS));
	BodyPtr blockBody(GCC_NEW Body(x, y, width, height));
	physicsSystem->registerBody(entity->id, blockBody);
	PhysicsComponentPtr physicsComponent(GCC_NEW PhysicsComponent(entity->id, blockBody, physicsSystem->physicsNotifier));

	GraphicsSystemPtr graphicsSystem = makeShared(mSystemManager->getSystemByType<GraphicsSystem>(SystemType::GRAPHICS));
	DrawablePtr textureDrawable(GCC_NEW ButtonDrawable(width, height, *mButtonConfig.get()));
	graphicsSystem->registerDrawable(entity->id, textureDrawable);
	DrawableComponentPtr drawableComponent(GCC_NEW DrawableComponent(entity->id, textureDrawable));

	ButtonComponentPtr buttonComponent(GCC_NEW ButtonComponent(entity->id));
	buttonComponent->setCallback(callback);

	entity->addComponent(physicsComponent);
	entity->addComponent(drawableComponent);
	entity->addComponent(buttonComponent);

	return entity;
}

EntityPtr WidgetFactory::createPanel(float x, float y, float width, float height) {
	EntitySystemPtr entitySystem = makeShared(mSystemManager->getSystemByType<EntitySystem>(SystemType::ENTITY));
	EntityPtr entity(GCC_NEW Entity());
	entitySystem->addEntity(entity);

	PhysicsSystemPtr physicsSystem = makeShared(mSystemManager->getSystemByType<PhysicsSystem>(SystemType::PHYSICS));
	BodyPtr blockBody(GCC_NEW Body(x, y, width, height));
	physicsSystem->registerBody(entity->id, blockBody);
	PhysicsComponentPtr physicsComponent(GCC_NEW PhysicsComponent(entity->id, blockBody, physicsSystem->physicsNotifier));

	GraphicsSystemPtr graphicsSystem = makeShared(mSystemManager->getSystemByType<GraphicsSystem>(SystemType::GRAPHICS));
	DrawablePtr textureDrawable(GCC_NEW PanelDrawable(width, height, *mPanelConfig.get()));
	graphicsSystem->registerDrawable(entity->id, textureDrawable);
	DrawableComponentPtr drawableComponent(GCC_NEW DrawableComponent(entity->id, textureDrawable));

	entity->addComponent(physicsComponent);
	entity->addComponent(drawableComponent);

	return entity;
}