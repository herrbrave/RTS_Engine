#include"Ui.h"

void applyButtonWithText(SystemManagerPtr systemManager, unsigned long entityId, float w, float h, const string& text, const string& font, const string& script, ButtonConfigPtr buttonConfig) {
	EntitySystemPtr entitySystem = systemManager->getSystemByType<EntitySystem>(SystemType::ENTITY);
	EntityPtr entity = entitySystem->getEntityById(entityId);

	GraphicsSystemPtr graphicsSystem = systemManager->getSystemByType<GraphicsSystem>(SystemType::GRAPHICS);

	ButtonComponentPtr buttonComponent;
	if (entity->getComponents().find(ComponentType::BUTTON_COMPONENT) != entity->getComponents().end()) {
		buttonComponent = entity->getComponentByType<ButtonComponent>(ComponentType::BUTTON_COMPONENT);
	}
	else {
		ButtonDrawablePtr buttonDrawable = std::make_shared<ButtonDrawable>(w, h, buttonConfig);
		graphicsSystem->registerDrawable(entity->id, static_cast<DrawablePtr>(buttonDrawable));

		InputListenerPtr inputListener = std::make_shared<InputListener>(entity->id);
		InputSystemPtr inputSystem = systemManager->getSystemByType<InputSystem>(SystemType::INPUT);
		inputSystem->registerEventListener(inputListener);

		LuaScriptPtr luaScript = std::make_shared<LuaScript>(script);

		ScriptLoadedData* scriptLoaded = GCC_NEW ScriptLoadedData(SDL_GetTicks(), entity->id, luaScript);
		EventManager::getInstance().pushEvent(scriptLoaded);

		buttonComponent = std::make_shared<ButtonComponent>(entity->id, buttonDrawable, inputListener, luaScript);

		entity->addComponent(buttonComponent);
	}

	buttonComponent->setText(text);
	buttonComponent->setFont(font);
}

void applyButtonWithIcon(SystemManagerPtr systemManager, unsigned long entityId, float w, float h, TexturePtr icon, const string& script, ButtonConfigPtr buttonConfig) {
	EntitySystemPtr entitySystem = systemManager->getSystemByType<EntitySystem>(SystemType::ENTITY);
	EntityPtr entity = entitySystem->getEntityById(entityId);

	GraphicsSystemPtr graphicsSystem = systemManager->getSystemByType<GraphicsSystem>(SystemType::GRAPHICS);

	ButtonComponentPtr buttonComponent;
	if (entity->getComponents().find(ComponentType::BUTTON_COMPONENT) != entity->getComponents().end()) {
		buttonComponent = entity->getComponentByType<ButtonComponent>(ComponentType::BUTTON_COMPONENT);
	}
	else {
		ButtonDrawablePtr buttonDrawable = std::make_shared<ButtonDrawable>(w, h, buttonConfig);
		graphicsSystem->registerDrawable(entity->id, static_cast<DrawablePtr>(buttonDrawable));

		InputListenerPtr inputListener = std::make_shared<InputListener>(entity->id);
		InputSystemPtr inputSystem = systemManager->getSystemByType<InputSystem>(SystemType::INPUT);
		inputSystem->registerEventListener(inputListener);

		LuaScriptPtr luaScript = std::make_shared<LuaScript>(script);

		ScriptLoadedData* scriptLoaded = GCC_NEW ScriptLoadedData(SDL_GetTicks(), entity->id, luaScript);
		EventManager::getInstance().pushEvent(scriptLoaded);

		buttonComponent = std::make_shared<ButtonComponent>(entity->id, buttonDrawable, inputListener, luaScript);

		entity->addComponent(buttonComponent);
	}

	buttonComponent->setIcon(icon);
}

void applyLabel(SystemManagerPtr systemManager, unsigned long entityId, const string& text, const string& font, Uint8 r, Uint8 g, Uint8 b, Uint8 a) {
	EntitySystemPtr entitySystem = systemManager->getSystemByType<EntitySystem>(SystemType::ENTITY);
	EntityPtr entity = entitySystem->getEntityById(entityId);

	GraphicsSystemPtr graphicsSystem = systemManager->getSystemByType<GraphicsSystem>(SystemType::GRAPHICS);

	LabelComponentPtr labelComponent;
	if (entity->getComponents().find(ComponentType::LABEL_COMPONENT) != entity->getComponents().end()) {
		labelComponent = entity->getComponentByType<LabelComponent>(ComponentType::LABEL_COMPONENT);
	}
	else {
		TextDrawablePtr textDrawable = std::make_shared<TextDrawable>(text, font);
		graphicsSystem->registerDrawable(entity->id, static_cast<DrawablePtr>(textDrawable));
		labelComponent = std::make_shared<LabelComponent>(entityId, textDrawable);

		entity->addComponent(labelComponent);
	}

	labelComponent->setText(text);
	labelComponent->setText(font);
}

void applyProgress(SystemManagerPtr systemManager, unsigned long entityId, float w, float h, unsigned int maxProgress, unsigned int currentProgress, const string& location) {
	EntitySystemPtr entitySystem = systemManager->getSystemByType<EntitySystem>(SystemType::ENTITY);
	EntityPtr entity = entitySystem->getEntityById(entityId);

	GraphicsSystemPtr graphicsSystem = systemManager->getSystemByType<GraphicsSystem>(SystemType::GRAPHICS);

	ProgressComponentPtr progressComponent;
	if (entity->getComponents().find(ComponentType::LABEL_COMPONENT) != entity->getComponents().end()) {
		progressComponent = entity->getComponentByType<ProgressComponent>(ComponentType::LABEL_COMPONENT);
	}
	else {
		ProgressBarDrawablePtr textureDrawable = std::make_shared<ProgressBarDrawable>(w, h, maxProgress, currentProgress);
		graphicsSystem->registerDrawable(entity->id, static_cast<DrawablePtr>(textureDrawable));
		progressComponent = std::make_shared<ProgressComponent>(entityId, textureDrawable);

		entity->addComponent(progressComponent);
	}

	progressComponent->setProgress(currentProgress, maxProgress);
}

void ProgressBarDrawable::draw(Graphics& graphicsRef, const Vector2f& position) {

	// padding is two pixels.
	int padding(2);
	float percentile((float)this->currentProgress / (float)this->progressMax);
	int innerwidth((this->width - 2 * padding) * percentile);

	float redScalar(((float)this->progressMax - (float)this->currentProgress) / (float)this->progressMax);
	float greenScalar((float)this->currentProgress / (float)this->progressMax);

	float innerWidth = 0, xOffset = 0, innerX = 0;

	if (this->currentProgress > 0) {
		innerWidth = this->width * percentile - (2 * padding);
		xOffset = (this->width - innerWidth) / 2;
		innerX = position.x - xOffset + padding;
	}

	graphicsRef.drawSquare(position.x, position.y, this->width, this->height, 64, 64, 64, 255);
	graphicsRef.drawSquare(innerX, position.y, innerWidth, this->height - (2 * padding), 255 * redScalar, 255 * greenScalar, 0, 255);
}

void LabelComponent::setText(const std::string& text) {
	this->textDrawable->setText(text);
}

void LabelComponent::setFont(const std::string& font) {
	this->textDrawable->setFont(font);
}

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

	ButtonConfig* buttonConfig = GCC_NEW ButtonConfig();
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

	PanelConfig* panelConfig = GCC_NEW PanelConfig();
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


SectionDrawable::SectionDrawable(float width, float height, Vector2f* positionOffset, TexturePtr texture) : Drawable(width, height) {
	mPosOffset = Vector2fPtr(positionOffset);
	mTexture = texture;
}

void SectionDrawable::draw(Graphics& graphicsRef, const Vector2f& position) {
	Vector2f translated = position + mPosOffset;
	graphicsRef.renderTexture(mTexture, translated.x, translated.y, width, height, 0, mColor->r, mColor->g, mColor->b, mColor->a);
}

ButtonDrawable::ButtonDrawable(float width, float height, ButtonConfigPtr buttonConfig) : Drawable(width, height), mButtonConfig(buttonConfig) {
	state = ButtonState::UP;
	mSections[ButtonState::UP] = vector<SectionDrawablePtr>();
	mSections[ButtonState::OVER] = vector<SectionDrawablePtr>();
	mSections[ButtonState::DOWN] = vector<SectionDrawablePtr>();

	float sectionWidth = buttonConfig->sectionWidth;
	float sectionHeight = buttonConfig->sectionHeight;
	float halfWidth = width / 2;
	float halfHeight = height / 2;
	this->iconWidth = width - (2.0f * sectionWidth);
	this->iconHeight = height - (2.0f * sectionHeight);

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
			Vector2f* posOffset(GCC_NEW Vector2f((halfWidth - (sectionWidth / 2)) * xDir, (halfHeight - (sectionHeight / 2)) * yDir));
			std::string name = names[count];

			SectionDrawablePtr upDrawable(GCC_NEW SectionDrawable(size[count][0], size[count][1], posOffset, buttonConfig->buttonUp.at(name)));
			SectionDrawablePtr overDrawable(GCC_NEW SectionDrawable(size[count][0], size[count][1], posOffset, buttonConfig->buttonOver.at(name)));
			SectionDrawablePtr downDrawable(GCC_NEW SectionDrawable(size[count][0], size[count][1], posOffset, buttonConfig->buttonDown.at(name)));
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
	if (this->mTexture != nullptr) {
		graphicsRef.renderTexture(this->mTexture, position.x, position.y, this->iconWidth, this->iconHeight, 0.0f, 255, 255, 255, 255);
	}
	if (this->text.size() > 0) {
		TextDrawable::draw(graphicsRef, position);
	}
}

void ButtonComponent::onMouseEvent(const MouseEvent& mouseEvent, SystemManagerPtr systemManager) {
	PhysicsSystemPtr physicsSystem = systemManager->getSystemByType<PhysicsSystem>(SystemType::PHYSICS);
	GraphicsSystemPtr graphicsSystem = systemManager->getSystemByType<GraphicsSystem>(SystemType::GRAPHICS);

	ButtonDrawablePtr buttonDrawable;
	vector<WeakDrawablePtr> drawables;
	graphicsSystem->getDrawableById(entityId, drawables);
	for (WeakDrawablePtr weakDrawablePtr : drawables) {
		DrawablePtr drawable = weakDrawablePtr.lock();
		if (drawable->getType() == DrawableType::BUTTON) {
			buttonDrawable = dynamic_pointer_cast<ButtonDrawable>(weakDrawablePtr.lock());
			break;
		}
	}
	assert(buttonDrawable != nullptr);

	if (mouseEvent.action == MouseAction::MOVE) {
		BodyPtr body = makeShared(physicsSystem->getBody(entityId));
		if (body->checkPoint(*mouseEvent.position)) {
			buttonDrawable->state = ButtonState::OVER;
		}
		else {
			buttonDrawable->state = ButtonState::UP;
		}
	}
	else if (mouseEvent.action == MouseAction::CLICK_DOWN) {
		if (buttonDrawable->state == ButtonState::OVER) {
			buttonDrawable->state = ButtonState::DOWN;
		}
	}
	else {
		if (buttonDrawable->state == ButtonState::DOWN) {
			buttonDrawable->state = ButtonState::OVER;
		}
	}
}

void ButtonComponent::setText(const std::string& text) {
	buttonDrawable->setText(text);
}

void ButtonComponent::setFont(const std::string& font) {
	buttonDrawable->setFont(font);
}

void ButtonComponent::setIcon(TexturePtr texture) {
	buttonDrawable->setTexture(texture);
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
			Vector2f* posOffset(GCC_NEW Vector2f((halfWidth - (sectionWidth / 2)) * xDir, (halfHeight -(sectionHeight / 2)) * yDir));
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


ItemPanelDrawable::ItemPanelDrawable(string name, float width, float height) : Drawable(width, height), name(name), drawableWidth(width), drawableHeight(height) {
	this->drawawbleArea = std::make_shared<Texture>(name, 0, 0, width, height);
}

void ItemPanelDrawable::update(Graphics& graphicsRef) {
	if (!forceRedraw) {
		return;
	}

	graphicsRef.drawToTexture(name);
	for (ItemPtr item : this->items) {
		for (TexturePtr texture : item->texture) {
			graphicsRef.renderTexture(texture, item->position->x, item->position->y, item->width, item->height, 0, 255, 255, 255, 255);
		}
	}
	graphicsRef.drawToScreen();

	forceRedraw = false;
}

void ItemPanelDrawable::addItem(ItemPtr item) {

	items.push_back(item);
	reconfigure();
	forceRedraw = true;
}

void ItemPanelDrawable::reconfigure() {
	float itemSize = drawableWidth / columns - ((columns + 1) * margin);

	for (int index = 0; index < items.size(); index++) {
		ItemPtr item = this->items.at(index);

		int row = index / columns;
		item->width = itemSize;
		item->height = itemSize;

		float column = index % columns;
		float x = column * itemSize + ((column + 1) * margin) + (itemSize / 2.0f);
		float y = row * itemSize + ((row + 1) * margin) + (itemSize / 2.0f);
		if (item->position == nullptr) {
			item->position.reset(GCC_NEW Vector2f(x, y));
		}
		else {
			item->position->set(x, y);
		}
	}

	forceRedraw = true;
}

void ItemPanelDrawable::draw(Graphics& graphicsRef, const Vector2f& position) {
	this->update(graphicsRef);

	graphicsRef.renderTexture(this->drawawbleArea, position.x, position.y, drawableWidth, drawableHeight, 0.0f, 255, 255, 255, 255);
}

ItemPanelComponent::ItemPanelComponent(unsigned long entityId, ItemPanelDrawablePtr panelDrawable) : Component(entityId, ComponentType::ITEM_PANEL_COMPONENT), panelDrawable(panelDrawable)  {

}

void ItemPanelComponent::setColumns(unsigned int columns) {
	this->panelDrawable->columns = columns;
	this->panelDrawable->reconfigure();
}

void ItemPanelComponent::pushTexture(TexturePtr texture, int x, int y) {
	this->panelDrawable->items.at(this->getIndex(x, y))->texture.push_back(texture);
	this->panelDrawable->reconfigure();
}

void ItemPanelComponent::popTexture(int x, int y) {
	this->panelDrawable->items.at(this->getIndex(x, y))->texture.pop_back();
	this->panelDrawable->reconfigure();
}

void ItemPanelComponent::addItem(ItemPtr item) {
	this->panelDrawable->addItem(item);
}

void ItemPanelComponent::popItem() {
	this->panelDrawable->items.pop_back();
	this->panelDrawable->reconfigure();
}

void ItemPanelComponent::removeItem(int x, int y) {
	this->panelDrawable->items.erase(this->panelDrawable->items.begin() + this->getIndex(x, y));
	this->panelDrawable->reconfigure();
}

int ItemPanelComponent::getIndex(int x, int y) {
	return (y / this->panelDrawable->columns) + x;
}

WidgetFactory::WidgetFactory(std::string buttonConfigPath, std::string panelConfigPath, SystemManagerPtr systemManager) : EntityFactory(systemManager) {
	mButtonConfig = createButtonConfig(buttonConfigPath, systemManager);
	mPanelConfig = createPanelConfig(panelConfigPath, systemManager);
}


EntityPtr WidgetFactory::createLabel(std::string text, std::string font, float x, float y) {
	EntityPtr entity = this->createPhysicsEntity(x, y, 1, 1);
	applyLabel(mSystemManager, entity->id, text, font, 255, 255, 255, 255);

	return entity;
}


EntityPtr WidgetFactory::createButtonWithText(const string& text, const string& font, const string& script, float x, float y, float width, float height) {
	EntityPtr entity = this->createPhysicsEntity(x, y, width, height);
	applyButtonWithText(mSystemManager, entity->id, width, height, text, font, script, mButtonConfig);

	return entity;
}


EntityPtr WidgetFactory::createButtonWithIcon(TexturePtr icon, const string& script, float x, float y, float width, float height) {
	EntityPtr entity = this->createPhysicsEntity(x, y, width, height);
	applyButtonWithIcon(mSystemManager, entity->id, width, height, icon, script, mButtonConfig);

	return entity;
}

EntityPtr WidgetFactory::createPanel(float x, float y, float width, float height) {
	EntitySystemPtr entitySystem = mSystemManager->getSystemByType<EntitySystem>(SystemType::ENTITY);
	EntityPtr entity(GCC_NEW Entity());
	entitySystem->addEntity(entity);

	PhysicsSystemPtr physicsSystem = mSystemManager->getSystemByType<PhysicsSystem>(SystemType::PHYSICS);
	BodyPtr blockBody(GCC_NEW Body(entity->id, x, y, width, height));
	physicsSystem->registerBody(entity->id, blockBody);
	PhysicsComponentPtr physicsComponent(GCC_NEW PhysicsComponent(entity->id, blockBody));

	GraphicsSystemPtr graphicsSystem = mSystemManager->getSystemByType<GraphicsSystem>(SystemType::GRAPHICS);
	DrawablePtr textureDrawable(GCC_NEW PanelDrawable(width, height, *mPanelConfig.get()));
	graphicsSystem->registerDrawable(entity->id, textureDrawable);
	DrawableComponentPtr drawableComponent(GCC_NEW DrawableComponent(entity->id, textureDrawable));

	entity->addComponent(physicsComponent);
	entity->addComponent(drawableComponent);

	return entity;
}


EntityPtr WidgetFactory::createProgressBar(float x, float y, float width, float height, unsigned int progressMax, unsigned int currentProgress) {
	EntityPtr entity = this->createPhysicsEntity(x, y, width, height);
	applyProgress(mSystemManager, entity->id, width, height, progressMax, currentProgress, "");

	return entity;
}