#include"EntityFactory.h"

EntityFactory::EntityFactory(SystemManagerPtr systemManager) {
	mSystemManager = systemManager;
}

EntityPtr EntityFactory::createDefault(float x, float y, float width, float height, Uint8 r, Uint8 g, Uint8 b, Uint8 a) {
	EntitySystemPtr entitySystem = makeShared(mSystemManager->getSystemByType<EntitySystem>(SystemType::ENTITY));
	EntityPtr entity(GCC_NEW Entity());
	entitySystem->addEntity(entity);

	PhysicsSystemPtr physicsSystem = makeShared(mSystemManager->getSystemByType<PhysicsSystem>(SystemType::PHYSICS));
	BodyPtr blockBody(GCC_NEW Body(entity->id, x, y, width, height));
	physicsSystem->registerBody(entity->id, blockBody);
	PhysicsComponentPtr physicsComponent(GCC_NEW PhysicsComponent(entity->id, blockBody));
	physicsComponent->setCollider(GCC_NEW Collider(x, y, width, height));

	GraphicsSystemPtr graphicsSystem = makeShared(mSystemManager->getSystemByType<GraphicsSystem>(SystemType::GRAPHICS));
	DrawablePtr blockDrawable(GCC_NEW BlockDrawable(width, height, r, g, b, a));
	graphicsSystem->registerDrawable(entity->id, blockDrawable);
	DrawableComponentPtr drawableComponent(GCC_NEW DrawableComponent(entity->id, blockDrawable));

	InputSystemPtr inputSystem = makeShared(mSystemManager->getSystemByType<InputSystem>(SystemType::INPUT));
	InputListenerPtr inputListener(GCC_NEW InputListener(entity->id));
	inputSystem->registerEventListener(inputListener);
	InputComponentPtr inputComponent(GCC_NEW InputComponent(entity->id, inputListener));

	entity->addComponent(ComponentPtr(physicsComponent));
	entity->addComponent(ComponentPtr(drawableComponent));
	entity->addComponent(ComponentPtr(inputComponent));

	return entity;
}

EntityPtr EntityFactory::createTexturedEntity(const string& assetTag, float x, float y, float width, float height, float tx, float ty, float w, float h, bool isCollidable) {
	EntitySystemPtr entitySystem = makeShared(mSystemManager->getSystemByType<EntitySystem>(SystemType::ENTITY));
	EntityPtr entity(GCC_NEW Entity());
	entitySystem->addEntity(entity);

	PhysicsSystemPtr physicsSystem = makeShared(mSystemManager->getSystemByType<PhysicsSystem>(SystemType::PHYSICS));
	BodyPtr blockBody(GCC_NEW Body(entity->id, x, y, width, height));
	physicsSystem->registerBody(entity->id, blockBody);
	PhysicsComponentPtr physicsComponent(GCC_NEW PhysicsComponent(entity->id, blockBody));
	if (isCollidable) {
		physicsComponent->setCollider(GCC_NEW Collider(x, y, width, height));
	}

	GraphicsSystemPtr graphicsSystem = makeShared(mSystemManager->getSystemByType<GraphicsSystem>(SystemType::GRAPHICS));
	TexturePtr texture(GCC_NEW Texture(assetTag, tx, ty, w, h));
	DrawablePtr textureDrawable(GCC_NEW TextureDrawable(texture));
	graphicsSystem->registerDrawable(entity->id, textureDrawable);
	DrawableComponentPtr drawableComponent(GCC_NEW DrawableComponent(entity->id, textureDrawable));

	entity->addComponent(ComponentPtr(physicsComponent));
	entity->addComponent(ComponentPtr(drawableComponent));

	return entity;
}

EntityPtr EntityFactory::createAnimatedEntity(const string& path, float x, float y, float width, float height) {
	EntitySystemPtr entitySystem = makeShared(mSystemManager->getSystemByType<EntitySystem>(SystemType::ENTITY));
	EntityPtr entity(GCC_NEW Entity());
	entitySystem->addEntity(entity);

	PhysicsSystemPtr physicsSystem = makeShared(mSystemManager->getSystemByType<PhysicsSystem>(SystemType::PHYSICS));
	BodyPtr blockBody(GCC_NEW Body(entity->id, x, y, width, height));
	physicsSystem->registerBody(entity->id, blockBody);
	PhysicsComponentPtr physicsComponent(GCC_NEW PhysicsComponent(entity->id, blockBody));

	GraphicsSystemPtr graphicsSystem = makeShared(mSystemManager->getSystemByType<GraphicsSystem>(SystemType::GRAPHICS));
	TexturePtr texture(GCC_NEW Texture(""));
	shared_ptr<TextureDrawable> textureDrawable(GCC_NEW TextureDrawable(texture));
	graphicsSystem->registerDrawable(entity->id, textureDrawable);
	AnimationSystemPtr animationSystem = makeShared(mSystemManager->getSystemByType<AnimationSystem>(SystemType::ANIMATION));
	AnimationSetPtr animationSet = animationSystem->createAnimationSet(path);
	AnimationHandlerPtr animationHandler(GCC_NEW AnimationHandler(textureDrawable, animationSet, animationSet->fps));
	animationSystem->registerAnimation(entity->id, animationHandler);
	AnimationComponentPtr animationComponent(GCC_NEW AnimationComponent(entity->id, animationHandler));

	entity->addComponent(ComponentPtr(physicsComponent));
	entity->addComponent(ComponentPtr(animationComponent));

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
			comp->setCollider(GCC_NEW Collider(body->getPosition().x, body->getPosition().y, body->getWidth(), body->getHeight()));
		}
		else if (componentId == ComponentType::TILE_COMPONENT) {
			entity->addComponent(ComponentPtr(GCC_NEW TileComponent(entity->id, component)));
		}
		else if (componentId == ComponentType::ANIMATION_COMPONENT) {
			AnimationComponent* animationComponent = GCC_NEW AnimationComponent(entity->id, component);
			entity->addComponent(ComponentPtr(animationComponent));
			graphicsSystem->registerDrawable(entity->id, animationComponent->animationHandler->textureDrawable); 
			animationSystem->registerAnimation(entity->id, animationComponent->animationHandler);
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

	PhysicsSystemPtr physicsSystem = makeShared(mSystemManager->getSystemByType<PhysicsSystem>(SystemType::PHYSICS));
	BodyPtr blockBody(GCC_NEW Body(entity->id, x, y, width, height));
	physicsSystem->registerBody(entity->id, blockBody);
	PhysicsComponentPtr physicsComponent(GCC_NEW PhysicsComponent(entity->id, blockBody));
	physicsComponent->setCollider(GCC_NEW Collider(x, y, width, height));

	entity->addComponent(ComponentPtr(physicsComponent));

	return entity;
}