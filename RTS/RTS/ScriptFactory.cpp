#include"ScriptFactory.h"

LuaScriptPtr LuaScriptFactory::create(const string& scriptPath) {
	LuaScriptPtr script(GCC_NEW LuaScript(scriptPath));

	this->registerDrawable(script);
	this->registerEntity(script);
	this->registerFactory(script);
	this->registerPhysics(script);
	this->registerAnimation(script);
	this->registerInput(script);

	script->invoke("setup");

	return script;
}

void LuaScriptFactory::registerFactory(LuaScriptPtr& script) {
	script->state["createDefault"] = [this](int x, int y, int w, int h, int r, int g, int b, int a) -> int {
		EntityFactoryPtr entityFactory = makeShared<EntityFactory>(mEntityFactory);
		EntityPtr entity = entityFactory->createDefault(x, y, w, h, r, g, b, a);
		return (int)entity->id;
	};

	script->state["createTextured"] = [this](string tag, int x, int y, int width, int height, int tx, int ty, int w, int h, bool collidable) -> int {
		EntityFactoryPtr entityFactory = makeShared<EntityFactory>(mEntityFactory);
		SystemManagerPtr systemManager = makeShared<SystemManager>(mSystemManager);
		GraphicsSystemPtr graphicsSystem = makeShared<GraphicsSystem>(systemManager->getSystemByType<GraphicsSystem>(SystemType::GRAPHICS));
		graphicsSystem->addTexture(tag, tag);

		EntityPtr entity = entityFactory->createTexturedEntity(tag, x, y, width, height, tx, ty, w, h, collidable);
		return (int)entity->id;
	};
}

void LuaScriptFactory::registerEntity(LuaScriptPtr& script) {
	script->state["destroyEntity"] = [this](int entityId) {
		SystemManagerPtr systemManager = makeShared<SystemManager>(mSystemManager);

		EntitySystemPtr entitySystem = makeShared<EntitySystem>(systemManager->getSystemByType<EntitySystem>(SystemType::ENTITY));
		entitySystem->deregisterEntity(entityId);
	};
}

void LuaScriptFactory::registerPhysics(LuaScriptPtr& script) {

	script->state["Vector2f"].SetClass<LuaFriendlyVector2f, double, double>(
		"getX", &LuaFriendlyVector2f::getX,
		"getY", &LuaFriendlyVector2f::getY,
		"normalize", &LuaFriendlyVector2f::normalize,
		"magnitude", &LuaFriendlyVector2f::magnitude,
		"truncate", &LuaFriendlyVector2f::truncate,
		"scale", &LuaFriendlyVector2f::scale,
		"add", &LuaFriendlyVector2f::add,
		"subtract", &LuaFriendlyVector2f::subtract);

	script->state["setVelocity"] = [this](int entityId, double x, double y) {
		SystemManagerPtr systemManager = makeShared<SystemManager>(mSystemManager);

		EntitySystemPtr entitySystem = makeShared<EntitySystem>(systemManager->getSystemByType<EntitySystem>(SystemType::ENTITY));

		EntityPtr entity = makeShared<Entity>(entitySystem->getEntityById(entityId));

		PhysicsComponentPtr physicsComponent = makeShared<PhysicsComponent>(entity->getComponentByType<PhysicsComponent>(ComponentType::PHYSICS_COMPONENT));
		physicsComponent->setVelocity(Vector2f(x, y));
	};

	script->state["addVelocity"] = [this](int entityId, double x, double y) {
		SystemManagerPtr systemManager = makeShared<SystemManager>(mSystemManager);

		EntitySystemPtr entitySystem = makeShared<EntitySystem>(systemManager->getSystemByType<EntitySystem>(SystemType::ENTITY));

		EntityPtr entity = makeShared<Entity>(entitySystem->getEntityById(entityId));

		PhysicsComponentPtr physicsComponent = makeShared<PhysicsComponent>(entity->getComponentByType<PhysicsComponent>(ComponentType::PHYSICS_COMPONENT));
		physicsComponent->setVelocity(physicsComponent->getVelocity() + Vector2f(x, y));
	};

	script->state["setSpeed"] = [this](int entityId, double speed) {
		SystemManagerPtr systemManager = makeShared<SystemManager>(mSystemManager);

		EntitySystemPtr entitySystem = makeShared<EntitySystem>(systemManager->getSystemByType<EntitySystem>(SystemType::ENTITY));

		EntityPtr entity = makeShared<Entity>(entitySystem->getEntityById(entityId));

		PhysicsComponentPtr physicsComponent = makeShared<PhysicsComponent>(entity->getComponentByType<PhysicsComponent>(ComponentType::PHYSICS_COMPONENT));
		physicsComponent->setSpeed(speed);
	};

	script->state["getSpeed"] = [this](int entityId, double speed) -> double {
		SystemManagerPtr systemManager = makeShared<SystemManager>(mSystemManager);

		EntitySystemPtr entitySystem = makeShared<EntitySystem>(systemManager->getSystemByType<EntitySystem>(SystemType::ENTITY));

		EntityPtr entity = makeShared<Entity>(entitySystem->getEntityById(entityId));

		PhysicsComponentPtr physicsComponent = makeShared<PhysicsComponent>(entity->getComponentByType<PhysicsComponent>(ComponentType::PHYSICS_COMPONENT));
		return (double)physicsComponent->getSpeed();
	};

	script->state["getPosition"] = [this](int entityId) -> LuaFriendlyVector2f& {
		SystemManagerPtr systemManager = makeShared<SystemManager>(mSystemManager);

		EntitySystemPtr entitySystem = makeShared<EntitySystem>(systemManager->getSystemByType<EntitySystem>(SystemType::ENTITY));

		EntityPtr entity = makeShared<Entity>(entitySystem->getEntityById(entityId));

		PhysicsComponentPtr physicsComponent = makeShared<PhysicsComponent>(entity->getComponentByType<PhysicsComponent>(ComponentType::PHYSICS_COMPONENT));
		const Vector2f& position(physicsComponent->getPosition());
		LuaFriendlyVector2f* vec = GCC_NEW LuaFriendlyVector2f(position);

		return *vec;
	};

	script->state["getVelocity"] = [this](int entityId) -> LuaFriendlyVector2f& {
		SystemManagerPtr systemManager = makeShared<SystemManager>(mSystemManager);

		EntitySystemPtr entitySystem = makeShared<EntitySystem>(systemManager->getSystemByType<EntitySystem>(SystemType::ENTITY));

		EntityPtr entity = makeShared<Entity>(entitySystem->getEntityById(entityId));

		PhysicsComponentPtr physicsComponent = makeShared<PhysicsComponent>(entity->getComponentByType<PhysicsComponent>(ComponentType::PHYSICS_COMPONENT));
		return *(GCC_NEW LuaFriendlyVector2f(const_cast<Vector2f&>(physicsComponent->getVelocity())));
	};

	script->state["setTarget"] = [this](int entityId, double x, double y, double threshold) {
		SystemManagerPtr systemManager = makeShared<SystemManager>(mSystemManager);

		EntitySystemPtr entitySystem = makeShared<EntitySystem>(systemManager->getSystemByType<EntitySystem>(SystemType::ENTITY));

		EntityPtr entity = makeShared<Entity>(entitySystem->getEntityById(entityId));

		PhysicsComponentPtr physicsComponent = makeShared<PhysicsComponent>(entity->getComponentByType<PhysicsComponent>(ComponentType::PHYSICS_COMPONENT));
		Vector2fPtr position(GCC_NEW Vector2f(x, y));
		TargetPtr target(GCC_NEW PositionTarget(position));
		target->setThreshold(threshold);

		physicsComponent->setTarget(target);
	};

	EventDelegate destroyEntityDelegate([script](const EventData& eventData) {
		EntityCollisionEventData data = dynamic_cast<const EntityCollisionEventData&>(eventData);

		script->invoke("onCollision", (int)data.getCollidedEntityId(), (int)data.getColliderEntityId());
	});

	EventListenerDelegate destroyEntityListener(destroyEntityDelegate);
	EventManager::getInstance().addDelegate(destroyEntityListener, EventType::ENTITY_COLLISION_EVENT);

	script->deleters.push_back([destroyEntityListener]() {
		EventManager::getInstance().removeDelegate(destroyEntityListener, EventType::ENTITY_COLLISION_EVENT);
	});
}

void LuaScriptFactory::registerDrawable(LuaScriptPtr& script) {
	script->state["setTexture"] = [this](string tag, int entityId, double tx, double ty, double w, double h) {
		SystemManagerPtr systemManager = makeShared<SystemManager>(mSystemManager);

		GraphicsSystemPtr graphicsSystem = makeShared<GraphicsSystem>(systemManager->getSystemByType<GraphicsSystem>(SystemType::GRAPHICS));

		EntitySystemPtr entitySystem = makeShared<EntitySystem>(systemManager->getSystemByType<EntitySystem>(SystemType::ENTITY));

		EntityPtr entity = makeShared<Entity>(entitySystem->getEntityById(entityId));

		DrawableComponentPtr drawableComponent = makeShared<DrawableComponent>(entity->getComponentByType<DrawableComponent>(ComponentType::DRAWABLE_COMPONENT));
		DrawablePtr drawable = makeShared<Drawable>(drawableComponent->getDrawable());
		TextureDrawablePtr textureDrawable = dynamic_pointer_cast<TextureDrawable>(drawable);
		graphicsSystem->addTexture(tag, tag);
		TexturePtr texture(GCC_NEW Texture(tag, tx, ty, w, h));

		textureDrawable->setTexture(texture);
	};


	script->state["getZOrder"] = [this](int entityId) -> int {
		SystemManagerPtr systemManager = makeShared<SystemManager>(mSystemManager);
		EntitySystemPtr entitySystem = makeShared<EntitySystem>(systemManager->getSystemByType<EntitySystem>(SystemType::ENTITY));
		EntityPtr entity = makeShared<Entity>(entitySystem->getEntityById(entityId));

		DrawableComponentPtr drawableComponent = makeShared<DrawableComponent>(entity->getComponentByType<DrawableComponent>(ComponentType::DRAWABLE_COMPONENT));

		return (int)drawableComponent->getZOrder();
	};

	script->state["setZOrder"] = [this](int entityId, int order) {
		SystemManagerPtr systemManager = makeShared<SystemManager>(mSystemManager);
		EntitySystemPtr entitySystem = makeShared<EntitySystem>(systemManager->getSystemByType<EntitySystem>(SystemType::ENTITY));
		EntityPtr entity = makeShared<Entity>(entitySystem->getEntityById(entityId));

		DrawableComponentPtr drawableComponent = makeShared<DrawableComponent>(entity->getComponentByType<DrawableComponent>(ComponentType::DRAWABLE_COMPONENT));

		drawableComponent->setZOrder(order);
	};

	script->state["setSize"] = [this](int entityId, double w, double h) {
		SystemManagerPtr systemManager = makeShared<SystemManager>(mSystemManager);
		EntitySystemPtr entitySystem = makeShared<EntitySystem>(systemManager->getSystemByType<EntitySystem>(SystemType::ENTITY));
		EntityPtr entity = makeShared<Entity>(entitySystem->getEntityById(entityId));

		DrawableComponentPtr drawableComponent = makeShared<DrawableComponent>(entity->getComponentByType<DrawableComponent>(ComponentType::DRAWABLE_COMPONENT));

		drawableComponent->setSize(w, h);
	};
}

void LuaScriptFactory::registerAnimation(LuaScriptPtr& script) {
	script->state["setAnimation"] = [this](int entityId, string animationName) {
		SystemManagerPtr systemManager = makeShared<SystemManager>(mSystemManager);
		EntitySystemPtr entitySystem = makeShared<EntitySystem>(systemManager->getSystemByType<EntitySystem>(SystemType::ENTITY));
		EntityPtr entity = makeShared<Entity>(entitySystem->getEntityById(entityId));

		AnimationComponentPtr animationComponent = makeShared<AnimationComponent>(entity->getComponentByType<AnimationComponent>(ComponentType::ANIMATION_COMPONENT));

		animationComponent->animationHandler->setAnimation(animationName);
	};

	script->state["stopAnimation"] = [this](int entityId) {
		SystemManagerPtr systemManager = makeShared<SystemManager>(mSystemManager);
		EntitySystemPtr entitySystem = makeShared<EntitySystem>(systemManager->getSystemByType<EntitySystem>(SystemType::ENTITY));
		EntityPtr entity = makeShared<Entity>(entitySystem->getEntityById(entityId));

		AnimationComponentPtr animationComponent = makeShared<AnimationComponent>(entity->getComponentByType<AnimationComponent>(ComponentType::ANIMATION_COMPONENT));

		animationComponent->animationHandler->stop();
	};

	script->state["playAnimation"] = [this](int entityId) {
		SystemManagerPtr systemManager = makeShared<SystemManager>(mSystemManager);
		EntitySystemPtr entitySystem = makeShared<EntitySystem>(systemManager->getSystemByType<EntitySystem>(SystemType::ENTITY));
		EntityPtr entity = makeShared<Entity>(entitySystem->getEntityById(entityId));

		AnimationComponentPtr animationComponent = makeShared<AnimationComponent>(entity->getComponentByType<AnimationComponent>(ComponentType::ANIMATION_COMPONENT));

		animationComponent->animationHandler->play();
	};
}

void LuaScriptFactory::registerInput(LuaScriptPtr& script) {

	script->state["SDLK_SPACE"] = (int)SDLK_SPACE;
	script->state["SDLK_UP"] = (int)SDLK_UP;
	script->state["SDLK_RIGHT"] = (int)SDLK_RIGHT;
	script->state["SDLK_DOWN"] = (int)SDLK_DOWN;
	script->state["SDLK_LEFT"] = (int)SDLK_LEFT;
	script->state["SDLK_SPACE"] = (int)SDLK_SPACE;

	script->state["MOUSE_BUTTON_LEFT"] = static_cast<int>(MouseButton::LEFT);
	script->state["MOUSE_BUTTON_MIDDLE"] = static_cast<int>(MouseButton::MIDDLE);
	script->state["MOUSE_BUTTON_LEFT"] = static_cast<int>(MouseButton::RIGHT);

	EventDelegate mouseEventDelegate([script](const EventData& eventData) {
		MouseEventData data = dynamic_cast<const MouseEventData&>(eventData);

		if (data.action == MouseAction::CLICK_UP) {
			script->invoke("onMouseUp", (double)data.x, (double)data.y, static_cast<int>(data.button));
		}
		else {
			script->invoke("onMouseDown", (double)data.x, (double)data.y, static_cast<int>(data.button));
		}
		return false;
	});

	EventListenerDelegate mouseEventListener(mouseEventDelegate);
	EventManager::getInstance().addDelegate(mouseEventListener, EventType::MOUSE_EVENT);

	script->deleters.push_back([mouseEventListener]() {
		EventManager::getInstance().removeDelegate(mouseEventListener, EventType::MOUSE_EVENT);
	});

	EventDelegate keyEventDelegate([script](const EventData& eventData) {
		KeyEventData data = dynamic_cast<const KeyEventData&>(eventData);

		if (data.action == KeyAction::UP) {
			script->invoke("onKeyUp", (int)data.key, data.ctrl, data.shft);
		}
		else {
			script->invoke("onKeyDown", (int)data.key, data.ctrl, data.shft);
		}
		return false;
	});

	EventListenerDelegate keyEventListener(keyEventDelegate);
	EventManager::getInstance().addDelegate(keyEventListener, EventType::KEY_EVENT);

	script->deleters.push_back([keyEventListener]() {
		EventManager::getInstance().removeDelegate(keyEventListener, EventType::KEY_EVENT);
	});
}