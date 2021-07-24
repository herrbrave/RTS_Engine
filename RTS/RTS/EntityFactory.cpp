#include"EntityFactory.h"


void applyDrawable(WeakSystemManagerPtr sm, unsigned long entityId, float x, float y, float width, float height, Uint8 r, Uint8 g, Uint8 b, Uint8 a) {
	SystemManagerPtr systemManager = makeShared(sm);
	EntitySystemPtr entitySystem = makeShared(systemManager->getSystemByType<EntitySystem>(SystemType::ENTITY));
	EntityPtr entity = makeShared(entitySystem->getEntityById(entityId));

	DrawableComponentPtr drawableComponent;
	if (entity->getComponents().find(ComponentType::DRAWABLE_COMPONENT) != entity->getComponents().end()) {
		drawableComponent = makeShared(entity->getComponentByType<DrawableComponent>(ComponentType::DRAWABLE_COMPONENT));

		GraphicsSystemPtr graphicsSystem = makeShared(systemManager->getSystemByType<GraphicsSystem>(SystemType::GRAPHICS));
		DrawablePtr blockDrawable(GCC_NEW BlockDrawable(width, height, r, g, b, a));
		graphicsSystem->deregisterDrawable(entity->id);
		graphicsSystem->registerDrawable(entity->id, blockDrawable);

		drawableComponent->setDrawable(blockDrawable);
	}
	else {
		GraphicsSystemPtr graphicsSystem = makeShared(systemManager->getSystemByType<GraphicsSystem>(SystemType::GRAPHICS)); 
		DrawablePtr blockDrawable(GCC_NEW BlockDrawable(width, height, r, g, b, a));
		graphicsSystem->registerDrawable(entity->id, blockDrawable);
		drawableComponent = DrawableComponentPtr(GCC_NEW DrawableComponent(entity->id, blockDrawable));

		entity->addComponent(ComponentPtr(drawableComponent));
	}
}

void applyDrawable(WeakSystemManagerPtr sm, unsigned long entityId, const string& texturePath, float width, float height, float tx, float ty, float w, float h) {
	SystemManagerPtr systemManager = makeShared(sm);
	EntitySystemPtr entitySystem = makeShared(systemManager->getSystemByType<EntitySystem>(SystemType::ENTITY));
	EntityPtr entity = makeShared(entitySystem->getEntityById(entityId));

	DrawableComponentPtr drawableComponent;
	if (entity->getComponents().find(ComponentType::DRAWABLE_COMPONENT) != entity->getComponents().end()) {
		drawableComponent = makeShared(entity->getComponentByType<DrawableComponent>(ComponentType::DRAWABLE_COMPONENT));
	}
	else {
		GraphicsSystemPtr graphicsSystem = makeShared(systemManager->getSystemByType<GraphicsSystem>(SystemType::GRAPHICS));
		TexturePtr texture(GCC_NEW Texture(texturePath, tx, ty, w, h));
		DrawablePtr textureDrawable(GCC_NEW TextureDrawable(texture));
		textureDrawable->setSize(width, height);
		graphicsSystem->registerDrawable(entity->id, textureDrawable);
		drawableComponent = DrawableComponentPtr(GCC_NEW DrawableComponent(entity->id, textureDrawable));

		entity->addComponent(ComponentPtr(drawableComponent));
	}

	TextureDrawablePtr drawable = dynamic_pointer_cast<TextureDrawable>(makeShared(drawableComponent->getDrawable()));
	TexturePtr texture(GCC_NEW Texture(texturePath, tx, ty, w, h));
	drawable->setTexture(texture);
}

void applyDrawable(WeakSystemManagerPtr sm, unsigned long entityId, TexturePtr texture) {
	SystemManagerPtr systemManager = makeShared(sm);
	EntitySystemPtr entitySystem = makeShared(systemManager->getSystemByType<EntitySystem>(SystemType::ENTITY));
	EntityPtr entity = makeShared(entitySystem->getEntityById(entityId));

	DrawableComponentPtr drawableComponent;
	if (entity->getComponents().find(ComponentType::DRAWABLE_COMPONENT) != entity->getComponents().end()) {
		drawableComponent = makeShared(entity->getComponentByType<DrawableComponent>(ComponentType::DRAWABLE_COMPONENT));
	}
	else {
		GraphicsSystemPtr graphicsSystem = makeShared(systemManager->getSystemByType<GraphicsSystem>(SystemType::GRAPHICS));
		DrawablePtr textureDrawable(GCC_NEW TextureDrawable(texture));
		graphicsSystem->registerDrawable(entity->id, textureDrawable);
		drawableComponent = DrawableComponentPtr(GCC_NEW DrawableComponent(entity->id, textureDrawable));

		entity->addComponent(ComponentPtr(drawableComponent));
	}

	TextureDrawablePtr drawable = dynamic_pointer_cast<TextureDrawable>(makeShared(drawableComponent->getDrawable()));
	drawable->setTexture(texture);
}

void applyPhysics(WeakSystemManagerPtr sm, unsigned long entityId, float x, float y, float w, float h) {
	SystemManagerPtr systemManager = makeShared(sm);
	EntitySystemPtr entitySystem = makeShared(systemManager->getSystemByType<EntitySystem>(SystemType::ENTITY));
	EntityPtr entity = makeShared(entitySystem->getEntityById(entityId));

	PhysicsSystemPtr physicsSystem = makeShared(systemManager->getSystemByType<PhysicsSystem>(SystemType::PHYSICS));

	PhysicsComponentPtr physicsComponent;
	if (entity->getComponents().find(ComponentType::PHYSICS_COMPONENT) != entity->getComponents().end()) {
		physicsComponent = makeShared(entity->getComponentByType<PhysicsComponent>(ComponentType::PHYSICS_COMPONENT));
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

void applyPhysics(WeakSystemManagerPtr sm, unsigned long entityId, float x, float y, float w, float h, ColliderShapePtr collider) {
	SystemManagerPtr systemManager = makeShared(sm);
	EntitySystemPtr entitySystem = makeShared(systemManager->getSystemByType<EntitySystem>(SystemType::ENTITY));
	EntityPtr entity = makeShared(entitySystem->getEntityById(entityId));

	PhysicsSystemPtr physicsSystem = makeShared(systemManager->getSystemByType<PhysicsSystem>(SystemType::PHYSICS));

	PhysicsComponentPtr physicsComponent;
	if (entity->getComponents().find(ComponentType::PHYSICS_COMPONENT) != entity->getComponents().end()) {
		physicsComponent = makeShared(entity->getComponentByType<PhysicsComponent>(ComponentType::PHYSICS_COMPONENT));
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

void applyInput(WeakSystemManagerPtr sm, unsigned long entityId, Input input, std::function<bool(EventPtr)>& callback) {
	SystemManagerPtr systemManager = makeShared(sm);
	EntitySystemPtr entitySystem = makeShared(systemManager->getSystemByType<EntitySystem>(SystemType::ENTITY));
	EntityPtr entity = makeShared(entitySystem->getEntityById(entityId));

	InputComponentPtr inputComponent;
	if (entity->getComponents().find(ComponentType::INPUT_COMPONENT) != entity->getComponents().end()) {
		inputComponent = makeShared(entity->getComponentByType<InputComponent>(ComponentType::INPUT_COMPONENT));
	}
	else {
		InputSystemPtr inputSystem = makeShared(systemManager->getSystemByType<InputSystem>(SystemType::INPUT));
		InputListenerPtr inputListener(GCC_NEW InputListener(entity->id));
		inputSystem->registerEventListener(inputListener);
		inputComponent = InputComponentPtr(GCC_NEW InputComponent(entity->id, inputListener));
		entity->addComponent(inputComponent);
	}

	inputComponent->setInputCallback(input, callback);
}

void applyAnimation(WeakSystemManagerPtr sm, unsigned long entityId, const string& path) {
	SystemManagerPtr systemManager = makeShared(sm);
	EntitySystemPtr entitySystem = makeShared(systemManager->getSystemByType<EntitySystem>(SystemType::ENTITY));
	EntityPtr entity = makeShared(entitySystem->getEntityById(entityId));

	AnimationComponentPtr animationComponent;
	if (entity->getComponents().find(ComponentType::ANIMATION_COMPONENT) != entity->getComponents().end()) {
		animationComponent = makeShared(entity->getComponentByType<AnimationComponent>(ComponentType::ANIMATION_COMPONENT));

		AnimationSystemPtr animationSystem = makeShared(systemManager->getSystemByType<AnimationSystem>(SystemType::ANIMATION));
		AnimationSetPtr animationSet = animationSystem->loadAnimationSet(path);
		animationComponent->aninmationDrawable->animationHandler->setAnimationSet(animationSet);
	}
	else {
		GraphicsSystemPtr graphicsSystem = makeShared(systemManager->getSystemByType<GraphicsSystem>(SystemType::GRAPHICS));
		AnimationSystemPtr animationSystem = makeShared(systemManager->getSystemByType<AnimationSystem>(SystemType::ANIMATION));
		AnimationSetPtr animationSet = animationSystem->loadAnimationSet(path);
		AnimationHandlerPtr animationHandler(GCC_NEW AnimationHandler(animationSet, animationSet->fps));
		AnimationDrawablePtr aninmationDrawable = std::make_shared<AnimationDrawable>(animationHandler);
		graphicsSystem->registerDrawable(entity->id, aninmationDrawable);
		animationSystem->registerAnimation(entity->id, animationHandler);
		animationComponent = AnimationComponentPtr(GCC_NEW AnimationComponent(entity->id, aninmationDrawable));

		entity->addComponent(animationComponent);
	}
}

void applyAnimation(WeakSystemManagerPtr sm, unsigned long entityId, AnimationSetPtr animationSet) {
	SystemManagerPtr systemManager = makeShared(sm);
	EntitySystemPtr entitySystem = makeShared(systemManager->getSystemByType<EntitySystem>(SystemType::ENTITY));
	EntityPtr entity = makeShared(entitySystem->getEntityById(entityId));

	AnimationComponentPtr animationComponent;
	if (entity->getComponents().find(ComponentType::ANIMATION_COMPONENT) != entity->getComponents().end()) {
		animationComponent = makeShared(entity->getComponentByType<AnimationComponent>(ComponentType::ANIMATION_COMPONENT));

		AnimationSystemPtr animationSystem = makeShared(systemManager->getSystemByType<AnimationSystem>(SystemType::ANIMATION));
		animationComponent->aninmationDrawable->animationHandler->setAnimationSet(animationSet);
	}
	else {
		GraphicsSystemPtr graphicsSystem = makeShared(systemManager->getSystemByType<GraphicsSystem>(SystemType::GRAPHICS));
		AnimationSystemPtr animationSystem = makeShared(systemManager->getSystemByType<AnimationSystem>(SystemType::ANIMATION));
		AnimationHandlerPtr animationHandler(GCC_NEW AnimationHandler(animationSet, animationSet->fps));
		AnimationDrawablePtr aninmationDrawable = std::make_shared<AnimationDrawable>(animationHandler);
		animationSystem->registerAnimation(entity->id, animationHandler);
		graphicsSystem->registerDrawable(entity->id, aninmationDrawable);
		animationComponent = AnimationComponentPtr(GCC_NEW AnimationComponent(entity->id, aninmationDrawable));

		entity->addComponent(animationComponent);
	}
}

void applyScript(WeakSystemManagerPtr sm, unsigned long entityId, const string& script) {
	SystemManagerPtr systemManager = makeShared(sm);
	EntitySystemPtr entitySystem(makeShared(systemManager->getSystemByType<EntitySystem>(SystemType::ENTITY)));
	EntityPtr entity = makeShared(entitySystem->getEntityById(entityId));

	if (entity->getComponents().find(ComponentType::LUA_SCRIPT_COMPONENT) != entity->getComponents().end()) {
		ERR("Script " + script + " cannont be applied to entity");
		throw std::exception("Cannot add new script to entity that already has a script.");
	}

	LuaScriptSystemPtr luaScriptSystem = makeShared<LuaScriptSystem>(systemManager->getSystemByType<LuaScriptSystem>(SystemType::LUA_SCRIPT));
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
	EntitySystemPtr entitySystem = makeShared(mSystemManager->getSystemByType<EntitySystem>(SystemType::ENTITY));
	EntityPtr entity(GCC_NEW Entity());
	entitySystem->addEntity(entity);

	applyPhysics(this->mSystemManager, entity->id, x, y, width, height, ColliderShapePtr(GCC_NEW AABBColliderShape(std::make_shared<Vector2f>(x, y), width, height)));
	applyDrawable(this->mSystemManager, entity->id, x, y, width, height, r, g, b, a);

	return entity;
}

EntityPtr EntityFactory::createTexturedEntity(const string& assetTag, float x, float y, float width, float height, float tx, float ty, float w, float h, bool isCollidable) {
	EntitySystemPtr entitySystem = makeShared(mSystemManager->getSystemByType<EntitySystem>(SystemType::ENTITY));
	EntityPtr entity(GCC_NEW Entity());
	entitySystem->addEntity(entity);

	applyDrawable(this->mSystemManager, entity->id, assetTag, width, height, tx, ty, w, h);
	if (isCollidable) {
		applyPhysics(this->mSystemManager, entity->id, x, y, width, height, ColliderShapePtr(GCC_NEW AABBColliderShape(std::make_shared<Vector2f>(x, y), width, height)));
	}
	else {
		applyPhysics(this->mSystemManager, entity->id, x, y, width, height);
	}

	return entity;
}

EntityPtr EntityFactory::createAnimatedEntity(const string& path, float x, float y, float width, float height) {
	EntitySystemPtr entitySystem = makeShared(mSystemManager->getSystemByType<EntitySystem>(SystemType::ENTITY));
	EntityPtr entity(GCC_NEW Entity());
	entitySystem->addEntity(entity);

	applyPhysics(this->mSystemManager, entity->id, x, y, width, height);
	applyAnimation(this->mSystemManager, entity->id, path);

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

	EntitySystemPtr entitySystem = static_pointer_cast<EntitySystem>(mSystemManager->systems.at(SystemType::ENTITY));
	EntityPtr entity(GCC_NEW Entity());
	entitySystem->addEntity(entity);
	const rapidjson::Value& componentContainer = doc["components"];

	GraphicsSystemPtr graphicsSystem = static_pointer_cast<GraphicsSystem>(mSystemManager->systems.at(SystemType::GRAPHICS));
	PhysicsSystemPtr physicsSystem = static_pointer_cast<PhysicsSystem>(mSystemManager->systems.at(SystemType::PHYSICS));
	AnimationSystemPtr animationSystem = makeShared(mSystemManager->getSystemByType<AnimationSystem>(SystemType::ANIMATION));
	for (rapidjson::SizeType index = 0; index < componentContainer.Size(); index++) {
		const rapidjson::Value& component = componentContainer[index];
		ComponentType componentId = (ComponentType) component["componentId"].GetUint();

		if (componentId == ComponentType::DRAWABLE_COMPONENT) {
			DrawableComponentPtr comp(GCC_NEW DrawableComponent(entity->id, component));
			entity->addComponent(ComponentPtr(comp));
			graphicsSystem->registerDrawable(entity->id, makeShared(comp->getDrawable()));
		}
		else if (componentId == ComponentType::PHYSICS_COMPONENT) {
			PhysicsComponentPtr comp(GCC_NEW PhysicsComponent(entity->id, component));
			entity->addComponent(ComponentPtr(comp));

			BodyPtr body = makeShared(comp->getBody());
			physicsSystem->registerBody(entity->id, body);
			comp->setCollider(ColliderPtr(GCC_NEW Collider(GCC_NEW AABBColliderShape(std::make_shared<Vector2f>(body->getPosition().x, body->getPosition().y), body->getWidth(), body->getHeight()))));
		}
		else if (componentId == ComponentType::TILE_COMPONENT) {
			entity->addComponent(ComponentPtr(GCC_NEW TileComponent(entity->id, component)));
		}
		else if (componentId == ComponentType::ANIMATION_COMPONENT) {
			AnimationComponent* animationComponent = GCC_NEW AnimationComponent(entity->id, component);
			entity->addComponent(ComponentPtr(animationComponent));
			graphicsSystem->registerDrawable(entity->id, animationComponent->aninmationDrawable); 
			animationSystem->registerAnimation(entity->id, animationComponent->aninmationDrawable->animationHandler);
		}
		else if (componentId == ComponentType::LUA_SCRIPT_COMPONENT) {
			InputSystemPtr inputSystem(makeShared(mSystemManager->getSystemByType<InputSystem>(SystemType::INPUT)));
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

EntityPtr EntityFactory::createPhysicsEntity(float x, float y, float width, float height) {
	EntitySystemPtr entitySystem(makeShared(mSystemManager->getSystemByType<EntitySystem>(SystemType::ENTITY)));
	EntityPtr entity(GCC_NEW Entity());
	entitySystem->addEntity(entity);

	applyPhysics(this->mSystemManager, entity->id, x, y, width, height, ColliderShapePtr(GCC_NEW AABBColliderShape(std::make_shared<Vector2f>(x, y), width, height)));

	return entity;
}

EntityPtr EntityFactory::createScriptEntity(const string& path) {
	EntitySystemPtr entitySystem(makeShared(mSystemManager->getSystemByType<EntitySystem>(SystemType::ENTITY)));
	EntityPtr entity(GCC_NEW Entity());
	entitySystem->addEntity(entity);

	applyPhysics(this->mSystemManager, entity->id, -1, -1, 1, 1);
	applyScript(this->mSystemManager, entity->id, path);

	return entity;
}