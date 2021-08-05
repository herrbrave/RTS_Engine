#include"EntityFactory.h"


void applyDrawable(SystemManagerPtr systemManager, unsigned long entityId, float x, float y, float width, float height, Uint8 r, Uint8 g, Uint8 b, Uint8 a) {
	EntitySystemPtr entitySystem = systemManager->getSystemByType<EntitySystem>(SystemType::ENTITY);
	EntityPtr entity = entitySystem->getEntityById(entityId);

	DrawableComponentPtr drawableComponent;
	if (entity->getComponents().find(ComponentType::DRAWABLE_COMPONENT) != entity->getComponents().end()) {
		drawableComponent = entity->getComponentByType<DrawableComponent>(ComponentType::DRAWABLE_COMPONENT);

		GraphicsSystemPtr graphicsSystem = systemManager->getSystemByType<GraphicsSystem>(SystemType::GRAPHICS);
		DrawablePtr blockDrawable = std::make_shared<BlockDrawable>(width, height, r, g, b, a);
		graphicsSystem->deregisterDrawable(entity->id);
		graphicsSystem->registerDrawable(entity->id, blockDrawable);

		drawableComponent->setDrawable(blockDrawable);
	}
	else {
		GraphicsSystemPtr graphicsSystem = systemManager->getSystemByType<GraphicsSystem>(SystemType::GRAPHICS); 
		DrawablePtr blockDrawable = std::make_shared<BlockDrawable>(width, height, r, g, b, a);
		graphicsSystem->registerDrawable(entity->id, blockDrawable);
		drawableComponent = std::make_shared<DrawableComponent>(entity->id, blockDrawable);

		entity->addComponent(ComponentPtr(drawableComponent));
	}
}

void applyDrawable(SystemManagerPtr systemManager, unsigned long entityId, const string& texturePath, float width, float height, float tx, float ty, float w, float h) {
	EntitySystemPtr entitySystem = systemManager->getSystemByType<EntitySystem>(SystemType::ENTITY);
	EntityPtr entity = entitySystem->getEntityById(entityId);

	DrawableComponentPtr drawableComponent;
	if (entity->getComponents().find(ComponentType::DRAWABLE_COMPONENT) != entity->getComponents().end()) {
		drawableComponent = entity->getComponentByType<DrawableComponent>(ComponentType::DRAWABLE_COMPONENT);

		TextureDrawablePtr drawable = dynamic_pointer_cast<TextureDrawable>(drawableComponent->getDrawable());
		TexturePtr texture(GCC_NEW Texture(texturePath, tx, ty, w, h));
		drawable->setTexture(texture);
	}
	else {
		GraphicsSystemPtr graphicsSystem = systemManager->getSystemByType<GraphicsSystem>(SystemType::GRAPHICS);
		TexturePtr texture(GCC_NEW Texture(texturePath, tx, ty, w, h));
		DrawablePtr textureDrawable(GCC_NEW TextureDrawable(texture));
		textureDrawable->setSize(width, height);
		graphicsSystem->registerDrawable(entity->id, textureDrawable);
		drawableComponent = DrawableComponentPtr(GCC_NEW DrawableComponent(entity->id, textureDrawable));

		entity->addComponent(ComponentPtr(drawableComponent));
	}
}

void applyDrawable(SystemManagerPtr systemManager, unsigned long entityId, TexturePtr texture, float width, float height) {
	EntitySystemPtr entitySystem = systemManager->getSystemByType<EntitySystem>(SystemType::ENTITY);
	EntityPtr entity = entitySystem->getEntityById(entityId);

	DrawableComponentPtr drawableComponent;
	if (entity->getComponents().find(ComponentType::DRAWABLE_COMPONENT) != entity->getComponents().end()) {
		drawableComponent = entity->getComponentByType<DrawableComponent>(ComponentType::DRAWABLE_COMPONENT);
		drawableComponent->setSize(width, height);
	}
	else {
		GraphicsSystemPtr graphicsSystem = systemManager->getSystemByType<GraphicsSystem>(SystemType::GRAPHICS);
		DrawablePtr textureDrawable = std::make_shared<TextureDrawable>(texture);
		graphicsSystem->registerDrawable(entity->id, textureDrawable);
		drawableComponent = std::make_shared<DrawableComponent>(entity->id, textureDrawable);
		drawableComponent->setSize(width, height);

		entity->addComponent(ComponentPtr(drawableComponent));
	}

	TextureDrawablePtr drawable = dynamic_pointer_cast<TextureDrawable>(drawableComponent->getDrawable());
	drawable->setTexture(texture);
}

void applyPhysics(SystemManagerPtr systemManager, unsigned long entityId, float x, float y, float w, float h) {
	EntitySystemPtr entitySystem = systemManager->getSystemByType<EntitySystem>(SystemType::ENTITY);
	EntityPtr entity = entitySystem->getEntityById(entityId);

	PhysicsSystemPtr physicsSystem = systemManager->getSystemByType<PhysicsSystem>(SystemType::PHYSICS);

	PhysicsComponentPtr physicsComponent;
	if (entity->getComponents().find(ComponentType::PHYSICS_COMPONENT) != entity->getComponents().end()) {
		physicsComponent = entity->getComponentByType<PhysicsComponent>(ComponentType::PHYSICS_COMPONENT);
	}
	else {
		BodyPtr blockBody(GCC_NEW Body(entity->id, x, y, w, h));
		physicsSystem->registerBody(entity->id, blockBody);
		physicsComponent = PhysicsComponentPtr(GCC_NEW PhysicsComponent(entity->id, blockBody));
		entity->addComponent(physicsComponent);
	}

	Vector2f pos{ x, y };
	physicsComponent->setPosition(pos);
	physicsComponent->setSize(w, h);

	if (physicsComponent->isCollidable()) {
		physicsComponent->setCollider(nullptr);
	}
}

void applyPhysics(SystemManagerPtr systemManager, unsigned long entityId, float x, float y, float w, float h, ColliderShapePtr collider) {
	EntitySystemPtr entitySystem = systemManager->getSystemByType<EntitySystem>(SystemType::ENTITY);
	EntityPtr entity = entitySystem->getEntityById(entityId);

	PhysicsSystemPtr physicsSystem = systemManager->getSystemByType<PhysicsSystem>(SystemType::PHYSICS);

	PhysicsComponentPtr physicsComponent;
	if (entity->getComponents().find(ComponentType::PHYSICS_COMPONENT) != entity->getComponents().end()) {
		physicsComponent = entity->getComponentByType<PhysicsComponent>(ComponentType::PHYSICS_COMPONENT);
	}
	else {
		BodyPtr blockBody(GCC_NEW Body(entity->id, x, y, w, h));
		physicsSystem->registerBody(entity->id, blockBody);
		physicsComponent = PhysicsComponentPtr(GCC_NEW PhysicsComponent(entity->id, blockBody));
		physicsComponent->setCollider(ColliderPtr(GCC_NEW Collider(GCC_NEW AABBColliderShape(std::make_shared<Vector2f>(x, y), w, h))));
		entity->addComponent(physicsComponent);
	}

	Vector2f pos{ x, y };
	physicsComponent->setPosition(pos);
	physicsComponent->setSize(w, h);
}

void applyInput(SystemManagerPtr systemManager, unsigned long entityId, Input input, std::function<bool(EventPtr)>& callback) {
	EntitySystemPtr entitySystem = systemManager->getSystemByType<EntitySystem>(SystemType::ENTITY);
	EntityPtr entity = entitySystem->getEntityById(entityId);

	InputComponentPtr inputComponent;
	if (entity->getComponents().find(ComponentType::INPUT_COMPONENT) != entity->getComponents().end()) {
		inputComponent = entity->getComponentByType<InputComponent>(ComponentType::INPUT_COMPONENT);
	}
	else {
		InputSystemPtr inputSystem = systemManager->getSystemByType<InputSystem>(SystemType::INPUT);
		InputListenerPtr inputListener(GCC_NEW InputListener(entity->id));
		inputSystem->registerEventListener(inputListener);
		inputComponent = InputComponentPtr(GCC_NEW InputComponent(entity->id, inputListener));
		entity->addComponent(inputComponent);
	}

	inputComponent->setInputCallback(input, callback);
}

void applyAnimation(SystemManagerPtr systemManager, unsigned long entityId, const string& path, int width, int height) {
	EntitySystemPtr entitySystem = systemManager->getSystemByType<EntitySystem>(SystemType::ENTITY);
	EntityPtr entity = entitySystem->getEntityById(entityId);

	AnimationComponentPtr animationComponent;
	if (entity->getComponents().find(ComponentType::ANIMATION_COMPONENT) != entity->getComponents().end()) {
		animationComponent = entity->getComponentByType<AnimationComponent>(ComponentType::ANIMATION_COMPONENT);

		AnimationSystemPtr animationSystem = systemManager->getSystemByType<AnimationSystem>(SystemType::ANIMATION);
		AnimationSetPtr animationSet = animationSystem->loadAnimationSet(path);
		animationComponent->aninmationDrawable->animationHandler->setAnimationSet(animationSet);
		animationComponent->aninmationDrawable->setSize(width, height);
	}
	else {
		GraphicsSystemPtr graphicsSystem = systemManager->getSystemByType<GraphicsSystem>(SystemType::GRAPHICS);
		AnimationSystemPtr animationSystem = systemManager->getSystemByType<AnimationSystem>(SystemType::ANIMATION);
		AnimationSetPtr animationSet = animationSystem->loadAnimationSet(path);

		graphicsSystem->addTexture(animationSet->spritesheet, animationSet->spritesheet);

		AnimationHandlerPtr animationHandler(GCC_NEW AnimationHandler(animationSet, animationSet->fps));
		AnimationDrawablePtr aninmationDrawable = std::make_shared<AnimationDrawable>(animationHandler);
		graphicsSystem->registerDrawable(entity->id, aninmationDrawable);
		animationSystem->registerAnimation(entity->id, animationHandler);
		animationComponent = AnimationComponentPtr(GCC_NEW AnimationComponent(entity->id, aninmationDrawable));
		animationComponent->aninmationDrawable->setSize(width, height);

		entity->addComponent(animationComponent);
	}
}

void applyAnimation(SystemManagerPtr systemManager, unsigned long entityId, AnimationSetPtr animationSet, int width, int height) {
	EntitySystemPtr entitySystem = systemManager->getSystemByType<EntitySystem>(SystemType::ENTITY);
	EntityPtr entity = entitySystem->getEntityById(entityId);

	AnimationComponentPtr animationComponent;
	if (entity->getComponents().find(ComponentType::ANIMATION_COMPONENT) != entity->getComponents().end()) {
		animationComponent = entity->getComponentByType<AnimationComponent>(ComponentType::ANIMATION_COMPONENT);

		AnimationSystemPtr animationSystem = systemManager->getSystemByType<AnimationSystem>(SystemType::ANIMATION);
		animationComponent->aninmationDrawable->animationHandler->setAnimationSet(animationSet);
		animationComponent->aninmationDrawable->setSize(width, height);
	}
	else {
		GraphicsSystemPtr graphicsSystem = systemManager->getSystemByType<GraphicsSystem>(SystemType::GRAPHICS);
		AnimationSystemPtr animationSystem = systemManager->getSystemByType<AnimationSystem>(SystemType::ANIMATION);
		AnimationHandlerPtr animationHandler(GCC_NEW AnimationHandler(animationSet, animationSet->fps));
		AnimationDrawablePtr aninmationDrawable = std::make_shared<AnimationDrawable>(animationHandler);
		animationSystem->registerAnimation(entity->id, animationHandler);
		graphicsSystem->registerDrawable(entity->id, aninmationDrawable);
		animationComponent = AnimationComponentPtr(GCC_NEW AnimationComponent(entity->id, aninmationDrawable));
		animationComponent->aninmationDrawable->setSize(width, height);

		entity->addComponent(animationComponent);
	}
}

void applyScript(SystemManagerPtr systemManager, unsigned long entityId, const string& script) {
	EntitySystemPtr entitySystem = systemManager->getSystemByType<EntitySystem>(SystemType::ENTITY);
	EntityPtr entity = entitySystem->getEntityById(entityId);

	if (entity->getComponents().find(ComponentType::LUA_SCRIPT_COMPONENT) != entity->getComponents().end()) {
		ERR("Script " + script + " cannont be applied to entity");
		throw std::exception("Cannot add new script to entity that already has a script.");
	}

	LuaScriptSystemPtr luaScriptSystem = systemManager->getSystemByType<LuaScriptSystem>(SystemType::LUA_SCRIPT);
	LuaScriptPtr luaScript(GCC_NEW LuaScript(script));

	LuaScriptComponentPtr scriptComponent(GCC_NEW LuaScriptComponent(entity->id, luaScript));
	ScriptLoadedData* scriptLoaded = GCC_NEW ScriptLoadedData(SDL_GetTicks(), entity->id, luaScript);
	EventManager::getInstance().pushEvent(scriptLoaded);
	entity->addComponent(ComponentPtr(scriptComponent));
}



EntityFactory::EntityFactory(SystemManagerPtr systemManager) {
	mSystemManager = systemManager;
}

EntityPtr EntityFactory::createDefault(float x, float y, float width, float height, Uint8 r, Uint8 g, Uint8 b, Uint8 a) {
	EntitySystemPtr entitySystem = mSystemManager->getSystemByType<EntitySystem>(SystemType::ENTITY);
	EntityPtr entity(GCC_NEW Entity());
	entitySystem->addEntity(entity);

	applyPhysics((SystemManagerPtr) this->mSystemManager, entity->id, x, y, width, height, ColliderShapePtr(GCC_NEW AABBColliderShape(std::make_shared<Vector2f>(x, y), width, height)));
	applyDrawable((SystemManagerPtr)this->mSystemManager, entity->id, x, y, width, height, r, g, b, a);

	return entity;
}

EntityPtr EntityFactory::createTexturedEntity(const string& assetTag, float x, float y, float width, float height, float tx, float ty, float w, float h, bool isCollidable) {
	EntitySystemPtr entitySystem = mSystemManager->getSystemByType<EntitySystem>(SystemType::ENTITY);
	EntityPtr entity(GCC_NEW Entity());
	entitySystem->addEntity(entity);

	applyDrawable((SystemManagerPtr)this->mSystemManager, entity->id, assetTag, width, height, tx, ty, w, h);
	if (isCollidable) {
		applyPhysics((SystemManagerPtr)this->mSystemManager, entity->id, x, y, width, height, ColliderShapePtr(GCC_NEW AABBColliderShape(std::make_shared<Vector2f>(x, y), width, height)));
	}
	else {
		applyPhysics((SystemManagerPtr)this->mSystemManager, entity->id, x, y, width, height);
	}

	return entity;
}

EntityPtr EntityFactory::createAnimatedEntity(const string& path, float x, float y, float width, float height) {
	EntitySystemPtr entitySystem = mSystemManager->getSystemByType<EntitySystem>(SystemType::ENTITY);
	EntityPtr entity(GCC_NEW Entity());
	entitySystem->addEntity(entity);

	applyPhysics((SystemManagerPtr)this->mSystemManager, entity->id, x, y, width, height);
	applyAnimation((SystemManagerPtr)this->mSystemManager, entity->id, path, width, height);

	return entity;
}

EntityPtr EntityFactory::createFromSerialization(const string& path) {
	std::ifstream file(path);
	std::string line;
	std::string builder;
	while (std::getline(file, line)) {
		builder.append(line);
	}
	file.close();

	rapidjson::Document doc;
	doc.Parse(builder.c_str());

	EntitySystemPtr entitySystem = mSystemManager->getSystemByType<EntitySystem>(SystemType::ENTITY);
	EntityPtr entity(GCC_NEW Entity());
	entitySystem->addEntity(entity);
	const rapidjson::Value& componentContainer = doc["components"];

	GraphicsSystemPtr graphicsSystem = mSystemManager->getSystemByType<GraphicsSystem>(SystemType::GRAPHICS);
	PhysicsSystemPtr physicsSystem = mSystemManager->getSystemByType<PhysicsSystem>(SystemType::PHYSICS);
	AnimationSystemPtr animationSystem = mSystemManager->getSystemByType<AnimationSystem>(SystemType::ANIMATION);
	for (rapidjson::SizeType index = 0; index < componentContainer.Size(); index++) {
		const rapidjson::Value& component = componentContainer[index];
		ComponentType componentId = (ComponentType) component["componentId"].GetUint();

		if (componentId == ComponentType::DRAWABLE_COMPONENT) {
			DrawableComponentPtr comp(GCC_NEW DrawableComponent(entity->id, component));
			entity->addComponent(ComponentPtr(comp));
			graphicsSystem->registerDrawable(entity->id, comp->getDrawable());
		}
		else if (componentId == ComponentType::PHYSICS_COMPONENT) {
			PhysicsComponentPtr comp(GCC_NEW PhysicsComponent(entity->id, component));
			entity->addComponent(ComponentPtr(comp));

			BodyPtr body = comp->getBody();
			physicsSystem->registerBody(entity->id, body);
			comp->setCollider(ColliderPtr(GCC_NEW Collider(GCC_NEW AABBColliderShape(std::make_shared<Vector2f>(body->getPosition().x, body->getPosition().y), body->getWidth(), body->getHeight()))));
		}
		else if (componentId == ComponentType::ANIMATION_COMPONENT) {
			AnimationComponent* animationComponent = GCC_NEW AnimationComponent(entity->id, component);
			entity->addComponent(ComponentPtr(animationComponent));
			graphicsSystem->registerDrawable(entity->id, animationComponent->aninmationDrawable); 
			animationSystem->registerAnimation(entity->id, animationComponent->aninmationDrawable->animationHandler);
		}
		else if (componentId == ComponentType::LUA_SCRIPT_COMPONENT) {
			InputSystemPtr inputSystem = mSystemManager->getSystemByType<InputSystem>(SystemType::INPUT);
			InputListenerPtr inputListener(GCC_NEW InputListener(entity->id));
			inputSystem->registerEventListener(inputListener);

			LuaScriptComponent* luaScriptComponent = GCC_NEW LuaScriptComponent(entity->id, component);
			entity->addComponent(ComponentPtr(luaScriptComponent));

			inputListener->eventCallbacks.emplace(Input::ON_MOUSE_ENTER, function<bool(EventPtr)>([luaScriptComponent](EventPtr evt) {
				luaScriptComponent->script->invoke("onMouseEnterEntity");
				return false;
			}));

			inputListener->eventCallbacks.emplace(Input::ON_MOUSE_EXIT, function<bool(EventPtr)>([luaScriptComponent](EventPtr evt) {
				luaScriptComponent->script->invoke("onMouseExitEntity");
				return false;
			}));

			inputListener->eventCallbacks.emplace(Input::ON_CLICK, function<bool(EventPtr)>([luaScriptComponent](EventPtr evt) {
				luaScriptComponent->script->invoke("onClickEntity", static_cast<int>(evt->mouseEvent->button));
				return false;
			}));

			inputListener->eventCallbacks.emplace(Input::ON_DRAG, function<bool(EventPtr)>([luaScriptComponent](EventPtr evt) {
				luaScriptComponent->script->invoke("onDragEntity", static_cast<int>(evt->mouseEvent->button));
				return false;
			}));

			inputListener->eventCallbacks.emplace(Input::ON_MOUSE_MOVE, function<bool(EventPtr)>([luaScriptComponent](EventPtr evt) {
				luaScriptComponent->script->invoke("onMouseMove", static_cast<int>(evt->mouseEvent->position->x), static_cast<int>(evt->mouseEvent->position->y), static_cast<int>(evt->mouseEvent->button));
				return false;
			}));
		}
		else {
			assert(false);
		}
	}

	return entity;
}

EntityPtr EntityFactory::createPhysicsEntity(float x, float y, float width, float height, bool isCollidable) {
	EntitySystemPtr entitySystem = mSystemManager->getSystemByType<EntitySystem>(SystemType::ENTITY);
	EntityPtr entity(GCC_NEW Entity());
	entitySystem->addEntity(entity);
	if (isCollidable) {
		applyPhysics((SystemManagerPtr)this->mSystemManager, entity->id, x, y, width, height, ColliderShapePtr(GCC_NEW AABBColliderShape(std::make_shared<Vector2f>(x, y), width, height)));
	}
	else {
		applyPhysics((SystemManagerPtr)this->mSystemManager, entity->id, x, y, width, height);
	}

	return entity;
}

EntityPtr EntityFactory::createScriptEntity(const string& path) {
	EntitySystemPtr entitySystem = mSystemManager->getSystemByType<EntitySystem>(SystemType::ENTITY);
	EntityPtr entity(GCC_NEW Entity());
	entitySystem->addEntity(entity);

	applyPhysics((SystemManagerPtr)this->mSystemManager, entity->id, -1, -1, 1, 1);
	applyScript((SystemManagerPtr)this->mSystemManager, entity->id, path);

	return entity;
}