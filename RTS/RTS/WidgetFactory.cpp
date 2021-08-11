#include"WidgetFactory.h"

void applyButtonWithText(SystemManagerPtr systemManager, unsigned long entityId, float w, float h, const string& text, const string& font, int fontSize, const string& script, ButtonConfigPtr buttonConfig) {
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
	buttonComponent->setFontSize(fontSize);
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

void applyLabel(SystemManagerPtr systemManager, unsigned long entityId, const string& text, const string& font, int fontSize, Uint8 r, Uint8 g, Uint8 b, Uint8 a) {
	EntitySystemPtr entitySystem = systemManager->getSystemByType<EntitySystem>(SystemType::ENTITY);
	EntityPtr entity = entitySystem->getEntityById(entityId);

	GraphicsSystemPtr graphicsSystem = systemManager->getSystemByType<GraphicsSystem>(SystemType::GRAPHICS);

	LabelComponentPtr labelComponent;
	if (entity->getComponents().find(ComponentType::LABEL_COMPONENT) != entity->getComponents().end()) {
		labelComponent = entity->getComponentByType<LabelComponent>(ComponentType::LABEL_COMPONENT);
	}
	else {
		TextDrawablePtr textDrawable = std::make_shared<TextDrawable>(text, font, fontSize);
		graphicsSystem->registerDrawable(entity->id, static_cast<DrawablePtr>(textDrawable));
		labelComponent = std::make_shared<LabelComponent>(entityId, textDrawable);

		entity->addComponent(labelComponent);
	}

	labelComponent->setText(text);
	labelComponent->setFont(font);
	labelComponent->setFontSize(fontSize);
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
void applyTextbox(SystemManagerPtr systemManager, unsigned long entityId, float w, float h, const string& text, const string& font, int fontSize, TextboxConfigPtr textboxConfig) {
	EntitySystemPtr entitySystem = systemManager->getSystemByType<EntitySystem>(SystemType::ENTITY);
	EntityPtr entity = entitySystem->getEntityById(entityId);

	GraphicsSystemPtr graphicsSystem = systemManager->getSystemByType<GraphicsSystem>(SystemType::GRAPHICS);

	TextboxComponentPtr textboxComponent;
	if (entity->getComponents().find(ComponentType::LABEL_COMPONENT) != entity->getComponents().end()) {
		textboxComponent = entity->getComponentByType<TextboxComponent>(ComponentType::TEXT_BOX_COMPONENT);
	}
	else {
		TextboxDrawablePtr textboxDrawable = std::make_shared<TextboxDrawable>(w, h, text, font, fontSize, textboxConfig);
		graphicsSystem->registerDrawable(entity->id, static_cast<DrawablePtr>(textboxDrawable));

		InputListenerPtr inputListener = std::make_shared<InputListener>(entity->id);
		InputSystemPtr inputSystem = systemManager->getSystemByType<InputSystem>(SystemType::INPUT);
		inputSystem->registerEventListener(inputListener);

		textboxComponent = std::make_shared<TextboxComponent>(entityId, textboxDrawable, inputListener);

		entity->addComponent(textboxComponent);
	}

	textboxComponent->setText(text);
	textboxComponent->setFont(font);
	textboxComponent->setFontSize(fontSize);
}

ButtonConfigPtr createButtonConfig(const rapidjson::Value& value, SystemManagerPtr systemManager) {
	std::string image = value["image"].GetString();
	GraphicsSystemPtr graphicsSystem = static_pointer_cast<GraphicsSystem>(systemManager->systems.at(SystemType::GRAPHICS));
	graphicsSystem->addTexture(image, image);

	ButtonConfigPtr buttonConfig = std::make_shared<ButtonConfig>();
	buttonConfig->sectionWidth = value["section_width"].GetInt();
	buttonConfig->sectionHeight = value["section_height"].GetInt();

	parseState(image, value["button_up"], buttonConfig->buttonUp);
	parseState(image, value["button_over"], buttonConfig->buttonOver);
	parseState(image, value["button_down"], buttonConfig->buttonDown);

	return buttonConfig;
}

PanelConfigPtr createPanelConfig(const rapidjson::Value& value, SystemManagerPtr systemManager) {
	std::string image = value["image"].GetString();
	GraphicsSystemPtr graphicsSystem = static_pointer_cast<GraphicsSystem>(systemManager->systems.at(SystemType::GRAPHICS));
	graphicsSystem->addTexture(image, image);

	PanelConfigPtr panelConfig = std::make_shared<PanelConfig>();
	panelConfig->sectionWidth = value["section_width"].GetInt();
	panelConfig->sectionHeight = value["section_height"].GetInt();

	parseState(image, value["sections"], panelConfig->panelSections);

	return panelConfig;
}

void parseState(const std::string& tag, const rapidjson::Value& button, std::unordered_map<std::string, TexturePtr>& stateMap) {
	for (auto it = button.MemberBegin(); it != button.MemberEnd(); it++) {
		std::string name = it->name.GetString();
		const rapidjson::Value& value = it->value;
		TexturePtr texture(GCC_NEW Texture(tag, value["x"].GetDouble(), value["y"].GetDouble(), value["w"].GetDouble(), value["h"].GetDouble()));
		stateMap.emplace(name, texture);
	}
}
TextboxConfigPtr createTextboxConfig(const rapidjson::Value& value, SystemManagerPtr systemManager) {
	std::string image = value["image"].GetString();
	GraphicsSystemPtr graphicsSystem = static_pointer_cast<GraphicsSystem>(systemManager->systems.at(SystemType::GRAPHICS));
	graphicsSystem->addTexture(image, image);

	TextboxConfigPtr textboxConfig = std::make_shared<TextboxConfig>();
	textboxConfig->sectionWidth = value["section_width"].GetInt();

	parseState(image, value["sections"], textboxConfig->textboxSections);

	return textboxConfig;
}

UIConfigPtr parseUIConfig(const std::string& path, SystemManagerPtr systemManager) {
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

	UIConfigPtr uiConfig = std::make_shared<UIConfig>();
	uiConfig->buttonConfig = createButtonConfig(doc["button"], systemManager);
	uiConfig->panelConfig = createPanelConfig(doc["panel"], systemManager);
	uiConfig->textboxConfig = createTextboxConfig(doc["textbox"], systemManager);
	uiConfig->font = doc["font"].GetString();
	uiConfig->fontTag = doc["fontTag"].GetString();

	return uiConfig;
}


WidgetFactory::WidgetFactory(const string& uiConfigPath, SystemManagerPtr systemManager) : EntityFactory(systemManager) {
	uiConfig = parseUIConfig(uiConfigPath, systemManager);

	GraphicsSystemPtr graphicsSystem = systemManager->getSystemByType<GraphicsSystem>(SystemType::GRAPHICS);
	for (int fontSize = MIN_FONT_SIZE; fontSize <= MAX_FONT_SIZE; fontSize++) {
		string tag = uiConfig->fontTag + "_" + std::to_string(fontSize);
		graphicsSystem->addFont(uiConfig->font, tag, fontSize);
	}
}


EntityPtr WidgetFactory::createLabel(std::string text, int fontSize, float x, float y) {
	EntityPtr entity = this->createPhysicsEntity(x, y, 1, 1);
	applyLabel(mSystemManager, entity->id, text, uiConfig->fontTag, fontSize, 255, 255, 255, 255);

	return entity;
}


EntityPtr WidgetFactory::createButtonWithText(const string& text, int fontSize, const string& script, float x, float y, float width, float height) {
	EntityPtr entity = this->createPhysicsEntity(x, y, width, height);
	applyButtonWithText(mSystemManager, entity->id, width, height, text, uiConfig->fontTag, fontSize, script, uiConfig->buttonConfig);

	return entity;
}


EntityPtr WidgetFactory::createButtonWithIcon(TexturePtr icon, const string& script, float x, float y, float width, float height) {
	EntityPtr entity = this->createPhysicsEntity(x, y, width, height);
	applyButtonWithIcon(mSystemManager, entity->id, width, height, icon, script, uiConfig->buttonConfig);

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
	DrawablePtr textureDrawable(GCC_NEW PanelDrawable(width, height, uiConfig->panelConfig));
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
EntityPtr WidgetFactory::createTextBox(float x, float y, float width, float height, const string& text, int fontSize) {
	EntityPtr entity = this->createPhysicsEntity(x, y, width, height);
	applyTextbox(mSystemManager, entity->id, width, height, text, uiConfig->fontTag, fontSize, this->uiConfig->textboxConfig);

	return entity;
}