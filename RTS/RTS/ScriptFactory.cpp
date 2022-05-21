#include"ScriptFactory.h"

LuaScriptPtr LuaScriptFactory::create(const string& scriptPath, unsigned long entityId) {
	LuaScriptPtr script(GCC_NEW LuaScript(scriptPath));

	this->initialize(*script, entityId);

	return script;
}

void LuaScriptFactory::initialize(LuaScript& script, unsigned long entityId) {

	script.state["entityId"] = (int)entityId;

	int DRAWABLE = script.state["registrar"]["DRAWABLE"];
	int ENTITY = script.state["registrar"]["ENTITY"];
	int FACTORY = script.state["registrar"]["FACTORY"];
	int PHYSICS = script.state["registrar"]["PHYSICS"];
	int ANIMATION = script.state["registrar"]["ANIMATION"];
	int INPUT = script.state["registrar"]["INPUT"];
	int SCRIPT = script.state["registrar"]["SCRIPT"];
	int UI = script.state["registrar"]["UI"];
	int ASSET = script.state["registrar"]["ASSET"];
	int CAMERA = script.state["registrar"]["CAMERA"];
	int MOUSE_MOVE = script.state["registrar"]["MOUSE_MOVE"];
	int WORLD = script.state["registrar"]["WORLD"];
	int SOUND = script.state["registrar"]["SOUND"];

	script.state["IntList"].SetClass<LuaFriendlyIntVector>(
		"size", &LuaFriendlyIntVector::size,
		"at", &LuaFriendlyIntVector::at,
		"push", &LuaFriendlyIntVector::push);

	script.state["StringList"].SetClass<LuaFriendlyStringVector>(
		"size", &LuaFriendlyStringVector::size,
		"at", &LuaFriendlyStringVector::at,
		"push", &LuaFriendlyStringVector::push);

	script.state["StringMap"].SetClass<LuaFriendlyStringMap>(
		"size", &LuaFriendlyStringMap::size,
		"at", &LuaFriendlyStringMap::at,
		"put", &LuaFriendlyStringMap::put);

	script.state["Vector2f"].SetClass<LuaFriendlyVector2f, double, double>(
		"getX", &LuaFriendlyVector2f::getX,
		"getY", &LuaFriendlyVector2f::getY,
		"setX", &LuaFriendlyVector2f::setX,
		"setY", &LuaFriendlyVector2f::setY,
		"normalize", &LuaFriendlyVector2f::normalize,
		"magnitude", &LuaFriendlyVector2f::magnitude,
		"truncate", &LuaFriendlyVector2f::truncate,
		"scale", &LuaFriendlyVector2f::scale,
		"multiply", &LuaFriendlyVector2f::muliply,
		"dot", &LuaFriendlyVector2f::dot,
		"add", &LuaFriendlyVector2f::add,
		"subtract", &LuaFriendlyVector2f::subtract,
		"moveToward", &LuaFriendlyVector2f::moveToward);

	script.state["Vector2fList"].SetClass<LuaFriendlyVector2fVector>(
		"size", &LuaFriendlyVector2fVector::size,
		"at", &LuaFriendlyVector2fVector::at,
		"push", &LuaFriendlyVector2fVector::push);

	this->registerGeneral(script);

	if (DRAWABLE) {
		this->registerDrawable(script);
	}
	if (ENTITY) {
		this->registerEntity(script);
	}
	if (FACTORY) {
		this->registerFactory(script);
	}
	if (PHYSICS) {
		this->registerPhysics(script);
	}
	if (ANIMATION) {
		this->registerAnimation(script);
	}
	if (INPUT) {
		this->registerInput(script, entityId);
	}
	if (SCRIPT) {
		this->registerScript(script);
	}
	if (UI) {
		this->registerUi(script);
	}
	if (ASSET) {
		this->registerAsset(script);
	}
	if (CAMERA) {
		this->registerCamera(script);
	}
	if (MOUSE_MOVE) {
		this->registerMouseMove(script);
	}
	if (WORLD) {
		this->registerWorld(script);
	}
	if (SOUND) {
		this->registerSound(script);
	}

	auto output = script.invoke("setup");
}

void LuaScriptFactory::clean() {
	//this->toDelete.clear();
}

void LuaScriptFactory::registerGeneral(LuaScript& script) {
	script.state["sendMessage"] = [this](int entityId, string message, string value) -> string {
		EntitySystemPtr entitySystem = systemManager->getSystemByType<EntitySystem>(SystemType::ENTITY);
		EntityPtr entity = makeShared<Entity>(entitySystem->getEntityById(entityId));

		LuaScriptComponentPtr scriptComponent = entity->getComponentByType<LuaScriptComponent>(ComponentType::LUA_SCRIPT_COMPONENT);

		return scriptComponent->script->invoke("onMessage", message, value);
	};

	script.state["broadcastMessage"] = [this](int entityId, string message, string value) {
		LuaScriptSystemPtr luaScriptSystem = systemManager->getSystemByType<LuaScriptSystem>(SystemType::LUA_SCRIPT);
		LuaScripts scripts = luaScriptSystem->getLuaScripts();
		for (auto entry : scripts) {
			if (entry.first == entityId) {
				continue;
			}

			auto script = entry.second;
			if ((*script).state["onBroadcast"].exists()) {
				(*script).invoke("onBroadcast", message, value);
			}
		}
	};

	script.state["loadData"] = [this](string path) {
		SystemManagerPtr systemManager = makeShared<SystemManager>(systemManager);
		DataSystemPtr dataSystem = systemManager->getSystemByType<DataSystem>(SystemType::DATA);
		dataSystem->load(path);
	};

	script.state["hasData"] = [this](string path, string key) -> bool {
		DataSystemPtr dataSystem = systemManager->getSystemByType<DataSystem>(SystemType::DATA);
		return dataSystem->hasData(path, key);
	};

	script.state["getData"] = [this](string path, string key) -> string {
		DataSystemPtr dataSystem = systemManager->getSystemByType<DataSystem>(SystemType::DATA);
		return dataSystem->getData(path, key);
	};

	script.state["putData"] = [this](string path, string key, string val) {
		DataSystemPtr dataSystem = systemManager->getSystemByType<DataSystem>(SystemType::DATA);
		dataSystem->putData(path, key, val);
	};

	script.state["saveData"] = [this](string path) {
		DataSystemPtr dataSystem = systemManager->getSystemByType<DataSystem>(SystemType::DATA);
		dataSystem->save(path);
	};

	script.state["closeData"] = [this](string path) {
		DataSystemPtr dataSystem = systemManager->getSystemByType<DataSystem>(SystemType::DATA);
		dataSystem->close(path);
	};

	script.state["include"] = [&script](string path) {
		script.state.Load(path);
	};

	script.state["random"] = [&script](int limit) -> int {
		return rand() % limit;
	};
}

void LuaScriptFactory::registerFactory(LuaScript& script) {
	script.state["createDefault"] = [this](int x, int y, int w, int h, int r, int g, int b, int a) -> int {
		EntityPtr entity = entityFactory->createDefault(x, y, w, h, r, g, b, a);
		return (int)entity->id;
	};

	script.state["createTextured"] = [this](string tag, int x, int y, int width, int height, int tx, int ty, int w, int h, bool collidable) -> int {
		GraphicsSystemPtr graphicsSystem = systemManager->getSystemByType<GraphicsSystem>(SystemType::GRAPHICS);
		graphicsSystem->addTexture(tag, tag);

		EntityPtr entity = entityFactory->createTexturedEntity(tag, x, y, width, height, tx, ty, w, h, collidable);
		return (int)entity->id;
	};

	script.state["createAnimated"] = [this](string path, int x, int y, int width, int height) -> int {
		EntityPtr entity = entityFactory->createAnimatedEntity(path, x, y, width, height);
		return (int)entity->id;
	};

	script.state["createPhysics"] = [this](int x, int y, int width, int height) -> int {
		EntityPtr entity = entityFactory->createPhysicsEntity(x, y, width, height);
		return (int)entity->id;
	};

	script.state["createSerialized"] = [this](string path) -> int {
		EntityPtr entity = entityFactory->createFromSerialization(path);

		return (int)entity->id;
	};
}

void LuaScriptFactory::registerEntity(LuaScript& script) {
	script.state["destroyEntity"] = [this](int entityId) {
		EntitySystemPtr entitySystem = systemManager->getSystemByType<EntitySystem>(SystemType::ENTITY);
		entitySystem->deregisterEntity(entityId);
	};


	script.state["setChild"] = [this](int parent, int child) {
		EntitySystemPtr entitySystem = systemManager->getSystemByType<EntitySystem>(SystemType::ENTITY);
		EntityPtr childEntity = makeShared<Entity>(entitySystem->getEntityById(child));

		EntityPtr parentEntity = makeShared<Entity>(entitySystem->getEntityById(parent));
		parentEntity->children.push_back(childEntity);
		childEntity->parent = parentEntity;
	};

	script.state["removeChild"] = [this](int parent, int child) {
		EntitySystemPtr entitySystem = systemManager->getSystemByType<EntitySystem>(SystemType::ENTITY);
		EntityPtr childEntity = makeShared<Entity>(entitySystem->getEntityById(child));

		EntityPtr parentEntity = makeShared<Entity>(entitySystem->getEntityById(parent));
		parentEntity->children.erase(std::find(parentEntity->children.begin(), parentEntity->children.end(), childEntity));
		childEntity->parent = nullptr;
	};
}

void LuaScriptFactory::registerPhysics(LuaScript& script) {

	script.state["moveAndCollide"] = [this](int entityId, double x, double y) {
		PhysicsSystemPtr physicsSystem = systemManager->getSystemByType<PhysicsSystem>(SystemType::PHYSICS);
		Vector2f delta(x, y);
		physicsSystem->moveAndCollide(entityId, delta);
	};

	script.state["moveAndSlide"] = [this](int entityId, double x, double y) -> LuaFriendlyVector2f& {
		PhysicsSystemPtr physicsSystem = systemManager->getSystemByType<PhysicsSystem>(SystemType::PHYSICS);
		Vector2f delta(x, y);
		Vector2fPtr newVel = physicsSystem->moveAndSlide(entityId, delta);
		LuaFriendlyVector2f* vec = GCC_NEW LuaFriendlyVector2f(newVel);

		return *vec;
	};

	script.state["setVelocity"] = [this](int entityId, double x, double y) {
		EntitySystemPtr entitySystem = systemManager->getSystemByType<EntitySystem>(SystemType::ENTITY);

		EntityPtr entity = entitySystem->getEntityById(entityId);

		PhysicsComponentPtr physicsComponent = entity->getComponentByType<PhysicsComponent>(ComponentType::PHYSICS_COMPONENT);
		Vector2f velocity(x, y);
		velocity.normalize();
		physicsComponent->setVelocity(velocity);
	};

	script.state["addVelocity"] = [this](int entityId, double x, double y) {
		EntitySystemPtr entitySystem = systemManager->getSystemByType<EntitySystem>(SystemType::ENTITY);

		EntityPtr entity = entitySystem->getEntityById(entityId);

		PhysicsComponentPtr physicsComponent = entity->getComponentByType<PhysicsComponent>(ComponentType::PHYSICS_COMPONENT);
		physicsComponent->setVelocity(physicsComponent->getVelocity() + Vector2f(x, y));
	};

	script.state["setSpeed"] = [this](int entityId, double speed) {
		EntitySystemPtr entitySystem = systemManager->getSystemByType<EntitySystem>(SystemType::ENTITY);

		EntityPtr entity = entitySystem->getEntityById(entityId);

		PhysicsComponentPtr physicsComponent = entity->getComponentByType<PhysicsComponent>(ComponentType::PHYSICS_COMPONENT);
		physicsComponent->setSpeed(speed);
	};

	script.state["getSpeed"] = [this](int entityId) -> double {
		EntitySystemPtr entitySystem = systemManager->getSystemByType<EntitySystem>(SystemType::ENTITY);

		EntityPtr entity = entitySystem->getEntityById(entityId);

		PhysicsComponentPtr physicsComponent = entity->getComponentByType<PhysicsComponent>(ComponentType::PHYSICS_COMPONENT);
		return (double)physicsComponent->getSpeed();
	};

	script.state["getPosition"] = [this](int entityId) -> LuaFriendlyVector2f& {
		EntitySystemPtr entitySystem = systemManager->getSystemByType<EntitySystem>(SystemType::ENTITY);

		EntityPtr entity = entitySystem->getEntityById(entityId);

		PhysicsComponentPtr physicsComponent = entity->getComponentByType<PhysicsComponent>(ComponentType::PHYSICS_COMPONENT);
		const Vector2f& position = physicsComponent->getPosition();
		LuaFriendlyVector2f* vec = GCC_NEW LuaFriendlyVector2f(position);

		return *vec;
	};

	script.state["getScreenPosition"] = [this](int entityId) -> LuaFriendlyVector2f& {
		EntitySystemPtr entitySystem = systemManager->getSystemByType<EntitySystem>(SystemType::ENTITY);
		GraphicsSystemPtr graphicsSystem = systemManager->getSystemByType<GraphicsSystem>(SystemType::GRAPHICS);
		CameraPtr camera = makeShared(graphicsSystem->getCamera());

		EntityPtr entity = entitySystem->getEntityById(entityId);

		PhysicsComponentPtr physicsComponent = entity->getComponentByType<PhysicsComponent>(ComponentType::PHYSICS_COMPONENT);
		Vector2f position;
		position.set(physicsComponent->getPosition().x - camera->position->x, physicsComponent->getPosition().y - camera->position->y);

		LuaFriendlyVector2f* vec = GCC_NEW LuaFriendlyVector2f(position);

		return *vec;
	};

	script.state["setPosition"] = [this](int entityId, double x, double y) {
		EntitySystemPtr entitySystem = systemManager->getSystemByType<EntitySystem>(SystemType::ENTITY);

		EntityPtr entity = entitySystem->getEntityById(entityId);

		PhysicsComponentPtr physicsComponent = entity->getComponentByType<PhysicsComponent>(ComponentType::PHYSICS_COMPONENT);
		Vector2f position(x, y);
		physicsComponent->setPosition(position);
	};

	script.state["getVelocity"] = [this](int entityId) -> LuaFriendlyVector2f& {
		EntitySystemPtr entitySystem = systemManager->getSystemByType<EntitySystem>(SystemType::ENTITY);

		EntityPtr entity = entitySystem->getEntityById(entityId);

		PhysicsComponentPtr physicsComponent = entity->getComponentByType<PhysicsComponent>(ComponentType::PHYSICS_COMPONENT);
		auto vec = *(GCC_NEW LuaFriendlyVector2f(const_cast<Vector2f&>(physicsComponent->getVelocity())));

		return vec;
	};

	script.state["checkCollisionsArea"] = [this](int x0, int y0, int x1, int y1) -> LuaFriendlyIntVector& {
		PhysicsSystemPtr physicsSystem = systemManager->getSystemByType<PhysicsSystem>(SystemType::PHYSICS);

		int upperLeftX = std::min(x0, x1);
		int upperLeftY = std::min(y0, y1);
		int lowerRightX = std::max(x0, x1);
		int lowerRightY = std::max(y0, y1);

		int width = lowerRightX - upperLeftX;
		int height = lowerRightY - upperLeftY;

		int posX = upperLeftX + (width / 2);
		int posY = upperLeftY + (height / 2);

		AABBColliderShapePtr colliderShape = std::make_shared<AABBColliderShape>(std::make_shared<Vector2f>(posX, posY), width, height);
		ColliderPtr collider = std::make_shared<Collider>(colliderShape.get());
		BodyPtr body(GCC_NEW Body(0, posX, posY, width, height));
		body->setCollider(ColliderPtr(collider));

		vector<BodyPtr> bodies;
		physicsSystem->quadTree->getCollidingBodies(body, bodies);

		LuaFriendlyIntVector* ids = GCC_NEW LuaFriendlyIntVector();
		for (auto& bodyPtr : bodies) {
			BodyPtr b = bodyPtr;
			ids->push(b->id);
		}

		return *ids;
	};

	script.state["checkCollisions"] = [this](int entityId) -> LuaFriendlyIntVector& {
		PhysicsSystemPtr physicsSystem = systemManager->getSystemByType<PhysicsSystem>(SystemType::PHYSICS);

		BodyPtr body = makeShared(physicsSystem->getBody(entityId));
		LuaFriendlyIntVector* ids = GCC_NEW LuaFriendlyIntVector();
		if (!body->isCollidable()) {
			return *ids;
		}

		EntitySystemPtr entitySystem = systemManager->getSystemByType<EntitySystem>(SystemType::ENTITY);

		EntityPtr entity = entitySystem->getEntityById(entityId);
		Vector2f originalPosition(body->getPosition());
		while (entity->parent != nullptr) {
			BodyPtr parentBody = makeShared(physicsSystem->getBody(entity->parent->id));
			*body->collider->colliderShape->position += *parentBody->position;

			entity = entity->parent;
		}

		vector<BodyPtr> bodies;
		physicsSystem->quadTree->getCollidingBodies(body, bodies);
		body->collider->colliderShape->position->set(originalPosition);

		for (auto& bodyPtr : bodies) {
			BodyPtr b = bodyPtr;
			ids->push(b->id);
		}
			
		return *ids;
	};

	script.state["setAABBCollision"] = [this](int entityId, int width, int height) {
		EntitySystemPtr entitySystem = systemManager->getSystemByType<EntitySystem>(SystemType::ENTITY);

		EntityPtr entity = entitySystem->getEntityById(entityId);

		PhysicsComponentPtr physicsComponent = entity->getComponentByType<PhysicsComponent>(ComponentType::PHYSICS_COMPONENT);
		BodyPtr body = physicsComponent->getBody();
		Vector2fPtr pos = std::make_shared<Vector2f>(body->position->x, body->position->y);
		body->setCollider(ColliderPtr(GCC_NEW Collider(GCC_NEW AABBColliderShape(pos, width, height))));

		EntityCollisionSetEventData* data = GCC_NEW EntityCollisionSetEventData(entityId, SDL_GetTicks());
		EventManager::getInstance().pushEvent(data);
	};

	script.state["setCircleCollision"] = [this](int entityId, int radius) {
		EntitySystemPtr entitySystem = systemManager->getSystemByType<EntitySystem>(SystemType::ENTITY);

		EntityPtr entity = entitySystem->getEntityById(entityId);

		PhysicsComponentPtr physicsComponent = entity->getComponentByType<PhysicsComponent>(ComponentType::PHYSICS_COMPONENT);
		BodyPtr body = physicsComponent->getBody();
		Vector2fPtr pos = std::make_shared<Vector2f>(body->position->x, body->position->y);
		body->setCollider(ColliderPtr(GCC_NEW Collider(GCC_NEW CircleColliderShape(pos, radius))));

		EntityCollisionSetEventData* data = GCC_NEW EntityCollisionSetEventData(entityId, SDL_GetTicks());
		EventManager::getInstance().pushEvent(data);
	};

	script.state["setOBBCollision"] = [this](int entityId, int width, int height, int angle) {
		EntitySystemPtr entitySystem = systemManager->getSystemByType<EntitySystem>(SystemType::ENTITY);

		EntityPtr entity = entitySystem->getEntityById(entityId);

		PhysicsComponentPtr physicsComponent = entity->getComponentByType<PhysicsComponent>(ComponentType::PHYSICS_COMPONENT);
		BodyPtr body = physicsComponent->getBody();
		Vector2fPtr pos = std::make_shared<Vector2f>(body->position->x, body->position->y);
		body->setCollider(ColliderPtr(GCC_NEW Collider(GCC_NEW OBBColliderShape(pos, width, height, angle))));

		EntityCollisionSetEventData* data = GCC_NEW EntityCollisionSetEventData(entityId, SDL_GetTicks());
		EventManager::getInstance().pushEvent(data);
	};

	script.state["setTarget"] = [this](int entityId, double x, double y, double threshold) {
		EntitySystemPtr entitySystem = systemManager->getSystemByType<EntitySystem>(SystemType::ENTITY);

		EntityPtr entity = entitySystem->getEntityById(entityId);

		PhysicsComponentPtr physicsComponent = entity->getComponentByType<PhysicsComponent>(ComponentType::PHYSICS_COMPONENT);
		Vector2fPtr position(GCC_NEW Vector2f(x, y));
		TargetPtr target(GCC_NEW PositionTarget(position));
		target->setThreshold(threshold);

		physicsComponent->setTarget(target);
	};

	script.state["setTag"] = [this](int entityId, string tag) {
		EntitySystemPtr entitySystem = systemManager->getSystemByType<EntitySystem>(SystemType::ENTITY);

		EntityPtr entity = entitySystem->getEntityById(entityId);

		PhysicsComponentPtr physicsComponent = entity->getComponentByType<PhysicsComponent>(ComponentType::PHYSICS_COMPONENT);
		physicsComponent->setTag(tag);
	};

	script.state["getTag"] = [this](int entityId) {
		EntitySystemPtr entitySystem = systemManager->getSystemByType<EntitySystem>(SystemType::ENTITY);

		if (!entitySystem->exists(entityId)) {
			return string("");
		}

		EntityPtr entity = entitySystem->getEntityById(entityId);

		PhysicsComponentPtr physicsComponent = entity->getComponentByType<PhysicsComponent>(ComponentType::PHYSICS_COMPONENT);
		return physicsComponent->getTag();
	};

	EventDelegate destroyEntityDelegate([&script](const EventData& eventData) {
		EntityCollisionEventData data = dynamic_cast<const EntityCollisionEventData&>(eventData);

		int id = script.state["entityId"];
		if (id == (int)data.getCollidedEntityId()) {
			script.invoke("onCollision", static_cast<int>((int)data.getColliderEntityId()));
		}
		else if (id == (int)data.getColliderEntityId()) {
			script.invoke("onCollision", static_cast<int>((int)data.getCollidedEntityId()));
		}
		});

	EventListenerDelegate destroyEntityListener(destroyEntityDelegate);
	EventManager::getInstance().addDelegate(destroyEntityListener, EventType::ENTITY_COLLISION_EVENT);

	script.deleters.push_back([destroyEntityListener]() {
		EventManager::getInstance().removeDelegate(destroyEntityListener, EventType::ENTITY_COLLISION_EVENT);
		});
}

void LuaScriptFactory::registerDrawable(LuaScript& script) {
	script.state["setTexture"] = [this](string tag, int entityId, double width, double height, double tx, double ty, double w, double h) {
		GraphicsSystemPtr graphicsSystem = systemManager->getSystemByType<GraphicsSystem>(SystemType::GRAPHICS);
		graphicsSystem->addTexture(tag, tag);

		applyDrawable(systemManager, entityId, tag, width, height, tx, ty, w, h);
	};

	script.state["setText"] = [this](string tag, int entityId, const string&text, int fontSize, Uint8 r, Uint8 g, Uint8 b) {
		applyText(systemManager, entityId, text, widgetFactory->getUIConfig()->fontTag, fontSize, r, g, b);
	};

	script.state["drawSquare"] = [this](int x0, int y0, int x1, int y1, int r, int g, int b, int a) {
		GraphicsSystemPtr graphicsSystem = systemManager->getSystemByType<GraphicsSystem>(SystemType::GRAPHICS);
	};

	script.state["getZOrder"] = [this](int entityId) -> int {
		EntitySystemPtr entitySystem = systemManager->getSystemByType<EntitySystem>(SystemType::ENTITY);
		EntityPtr entity = entitySystem->getEntityById(entityId);

		if (entity->getComponents().find(ComponentType::DRAWABLE_COMPONENT) != entity->getComponents().end()) {
			DrawableComponentPtr drawableComponent = entity->getComponentByType<DrawableComponent>(ComponentType::DRAWABLE_COMPONENT);

			return drawableComponent->getZOrder();
		}
		else if (entity->getComponents().find(ComponentType::ANIMATION_COMPONENT) != entity->getComponents().end()) {
			AnimationComponentPtr animationComponent = entity->getComponentByType<AnimationComponent>(ComponentType::ANIMATION_COMPONENT);
			return animationComponent->getZOrder();
		}

		return 0;
	};

	script.state["setZOrder"] = [this](int entityId, int order) {
		EntitySystemPtr entitySystem = systemManager->getSystemByType<EntitySystem>(SystemType::ENTITY);
		EntityPtr entity = entitySystem->getEntityById(entityId);

		if (entity->getComponents().find(ComponentType::DRAWABLE_COMPONENT) != entity->getComponents().end()) {
			DrawableComponentPtr drawableComponent = entity->getComponentByType<DrawableComponent>(ComponentType::DRAWABLE_COMPONENT);

			drawableComponent->setZOrder(order);
		}
	};

	script.state["setAnimationZOrder"] = [this](int entityId, int order) {
		EntitySystemPtr entitySystem = systemManager->getSystemByType<EntitySystem>(SystemType::ENTITY);
		EntityPtr entity = entitySystem->getEntityById(entityId);

		if (entity->getComponents().find(ComponentType::ANIMATION_COMPONENT) != entity->getComponents().end()) {
			AnimationComponentPtr animationComponent = entity->getComponentByType<AnimationComponent>(ComponentType::ANIMATION_COMPONENT);
			animationComponent->setZOrder(order);
		}
	};

	script.state["setParticleZOrder"] = [this](int entityId, int order) {
		EntitySystemPtr entitySystem = systemManager->getSystemByType<EntitySystem>(SystemType::ENTITY);
		EntityPtr entity = entitySystem->getEntityById(entityId);

		if (entity->getComponents().find(ComponentType::PARTICLE_COMPONENT) != entity->getComponents().end()) {
			ParticleCloudComponentPtr drawableComponent = entity->getComponentByType<ParticleCloudComponent>(ComponentType::PARTICLE_COMPONENT);

			drawableComponent->setZOrder(order);
		}
	};

	script.state["setSize"] = [this](int entityId, double w, double h) {
		EntitySystemPtr entitySystem = systemManager->getSystemByType<EntitySystem>(SystemType::ENTITY);
		EntityPtr entity = entitySystem->getEntityById(entityId);

		if (entity->getComponents().find(ComponentType::DRAWABLE_COMPONENT) != entity->getComponents().end()) {
			DrawableComponentPtr drawableComponent = entity->getComponentByType<DrawableComponent>(ComponentType::DRAWABLE_COMPONENT);

			drawableComponent->setSize(w, h);
		}
		else if (entity->getComponents().find(ComponentType::ANIMATION_COMPONENT) != entity->getComponents().end()) {
			AnimationComponentPtr animationComponent = entity->getComponentByType<AnimationComponent>(ComponentType::ANIMATION_COMPONENT);
			animationComponent->setSize(w, h);
		}
	};

	script.state["setAngle"] = [this](int entityId, double angle) {
		EntitySystemPtr entitySystem = systemManager->getSystemByType<EntitySystem>(SystemType::ENTITY);
		EntityPtr entity = entitySystem->getEntityById(entityId);

		if (entity->getComponents().find(ComponentType::DRAWABLE_COMPONENT) != entity->getComponents().end()) {
			DrawableComponentPtr drawableComponent = entity->getComponentByType<DrawableComponent>(ComponentType::DRAWABLE_COMPONENT);

			drawableComponent->setAngle(angle);
		}
		else if (entity->getComponents().find(ComponentType::ANIMATION_COMPONENT) != entity->getComponents().end()) {
			AnimationComponentPtr animationComponent = entity->getComponentByType<AnimationComponent>(ComponentType::ANIMATION_COMPONENT);
			animationComponent->setAngle(angle);
		}

		if (entity->getComponents().find(ComponentType::PHYSICS_COMPONENT) != entity->getComponents().end()) {
			PhysicsComponentPtr physicsComponent = entity->getComponentByType<PhysicsComponent>(ComponentType::PHYSICS_COMPONENT);
			BodyPtr body = physicsComponent->getBody();
			if (body->isCollidable() && body->collider->colliderShape->colliderType() == ColliderType::OBB) {
				OBBColliderShapePtr obb = dynamic_pointer_cast<OBBColliderShape>(body->collider->colliderShape);
				obb->setAngle(angle);
			}
		}
	};

	script.state["getAngle"] = [this](int entityId) {
		EntitySystemPtr entitySystem = systemManager->getSystemByType<EntitySystem>(SystemType::ENTITY);
		EntityPtr entity = entitySystem->getEntityById(entityId);

		if (entity->getComponents().find(ComponentType::DRAWABLE_COMPONENT) != entity->getComponents().end()) {
			DrawableComponentPtr drawableComponent = entity->getComponentByType<DrawableComponent>(ComponentType::DRAWABLE_COMPONENT);

			return (double)drawableComponent->getAngle();
		}
		else if (entity->getComponents().find(ComponentType::ANIMATION_COMPONENT) != entity->getComponents().end()) {
			AnimationComponentPtr animationComponent = entity->getComponentByType<AnimationComponent>(ComponentType::ANIMATION_COMPONENT);
			return (double)animationComponent->getAngle();
		}

		return 0.0;
	};

	script.state["setColor"] = [this](int entityId, int r, int g, int b, int a) {
		EntitySystemPtr entitySystem = systemManager->getSystemByType<EntitySystem>(SystemType::ENTITY);
		EntityPtr entity = entitySystem->getEntityById(entityId);

		if (entity->getComponents().find(ComponentType::DRAWABLE_COMPONENT) != entity->getComponents().end()) {
			DrawableComponentPtr drawableComponent = entity->getComponentByType<DrawableComponent>(ComponentType::DRAWABLE_COMPONENT);
			drawableComponent->setColor(r, g, b, a);
		}
		else if (entity->getComponents().find(ComponentType::ANIMATION_COMPONENT) != entity->getComponents().end()) {
			AnimationComponentPtr animationComponent = entity->getComponentByType<AnimationComponent>(ComponentType::ANIMATION_COMPONENT);
			animationComponent->setColor(r, g, b, a);
		}
	};

	script.state["addParticle"] = [this](int entityId, const string& animation, int particleLifeMillis, int spreadDist, double gravX, double gravY, int partW, int partH, double particleSpeed) {
		EntitySystemPtr entitySystem = systemManager->getSystemByType<EntitySystem>(SystemType::ENTITY);
		EntityPtr entity = entitySystem->getEntityById(entityId);

		Vector2f grav(gravX, gravY);
		applyParticle(systemManager, entityId, animation, particleLifeMillis, grav, partW, partH, spreadDist, particleSpeed);
	};

	script.state["playParticle"] = [this](int entityId) {
		EntitySystemPtr entitySystem = systemManager->getSystemByType<EntitySystem>(SystemType::ENTITY);
		EntityPtr entity = entitySystem->getEntityById(entityId);

		ParticleCloudComponentPtr particleCloud = entity->getComponentByType<ParticleCloudComponent>(ComponentType::PARTICLE_COMPONENT);
		particleCloud->play();
	};

	script.state["stopParticle"] = [this](int entityId) {
		EntitySystemPtr entitySystem = systemManager->getSystemByType<EntitySystem>(SystemType::ENTITY);
		EntityPtr entity = entitySystem->getEntityById(entityId);

		ParticleCloudComponentPtr particleCloud = entity->getComponentByType<ParticleCloudComponent>(ComponentType::PARTICLE_COMPONENT);
		particleCloud->stop();
	};
}

void LuaScriptFactory::registerAnimation(LuaScript& script) {
	script.state["attachAnimationSet"] = [this](int entityId, string path) {
		EntitySystemPtr entitySystem = systemManager->getSystemByType<EntitySystem>(SystemType::ENTITY);
		EntityPtr entity = entitySystem->getEntityById(entityId);

		PhysicsComponentPtr physicsComponent = entity->getComponentByType<PhysicsComponent>(ComponentType::PHYSICS_COMPONENT);

		applyAnimation(systemManager, entityId, path, physicsComponent->getWidth(), physicsComponent->getHeight());
	};

	script.state["attachAsepriteAnimationSet"] = [this](int entityId, string path) {
		EntitySystemPtr entitySystem = systemManager->getSystemByType<EntitySystem>(SystemType::ENTITY);
		EntityPtr entity = entitySystem->getEntityById(entityId);

		PhysicsComponentPtr physicsComponent = entity->getComponentByType<PhysicsComponent>(ComponentType::PHYSICS_COMPONENT);

		AssetSystemPtr assetSystem = systemManager->getSystemByType<AssetSystem>(SystemType::ASSET);
		AssetPtr asset = assetSystem->getAsset(path);
		if (asset == nullptr) {
			AnimationSetPtr animationSet = loadAsepriteAnimation(path);
			asset = std::make_shared<Asset>(VoidPtr(animationSet), path, path);
		}
		applyAnimation(systemManager, entityId, asset->getAsset<AnimationSet>(), physicsComponent->getWidth(), physicsComponent->getHeight());
	};

	script.state["setAnimation"] = [this](int entityId, string animationName) {
		EntitySystemPtr entitySystem = systemManager->getSystemByType<EntitySystem>(SystemType::ENTITY);
		EntityPtr entity = entitySystem->getEntityById(entityId);

		AnimationComponentPtr animationComponent = entity->getComponentByType<AnimationComponent>(ComponentType::ANIMATION_COMPONENT);

		animationComponent->setAnimation(animationName);
	};

	script.state["stopAnimation"] = [this](int entityId) {
		EntitySystemPtr entitySystem = systemManager->getSystemByType<EntitySystem>(SystemType::ENTITY);
		EntityPtr entity = entitySystem->getEntityById(entityId);

		AnimationComponentPtr animationComponent = entity->getComponentByType<AnimationComponent>(ComponentType::ANIMATION_COMPONENT);

		animationComponent->stop();
	};

	script.state["playAnimation"] = [this](int entityId) {
		EntitySystemPtr entitySystem = systemManager->getSystemByType<EntitySystem>(SystemType::ENTITY);
		EntityPtr entity = entitySystem->getEntityById(entityId);

		AnimationComponentPtr animationComponent = entity->getComponentByType<AnimationComponent>(ComponentType::ANIMATION_COMPONENT);

		animationComponent->play();
	};

	script.state["loopAnimation"] = [this](int entityId) {
		EntitySystemPtr entitySystem = systemManager->getSystemByType<EntitySystem>(SystemType::ENTITY);
		EntityPtr entity = entitySystem->getEntityById(entityId);

		AnimationComponentPtr animationComponent = entity->getComponentByType<AnimationComponent>(ComponentType::ANIMATION_COMPONENT);

		animationComponent->loop();
	};

	script.state["isAnimationPlaying"] = [this](int entityId) {
		EntitySystemPtr entitySystem = systemManager->getSystemByType<EntitySystem>(SystemType::ENTITY);
		EntityPtr entity = entitySystem->getEntityById(entityId);

		AnimationComponentPtr animationComponent = entity->getComponentByType<AnimationComponent>(ComponentType::ANIMATION_COMPONENT);

		return animationComponent->getAnimationState() == AnimationState::PLAYING || animationComponent->getAnimationState() == AnimationState::LOOPING;
	};
}

void LuaScriptFactory::registerInput(LuaScript& script, unsigned long entityId) {

	script.state["SDLK_SPACE"] = (int)SDLK_SPACE;
	script.state["SDLK_UP"] = (int)SDLK_UP;
	script.state["SDLK_RIGHT"] = (int)SDLK_RIGHT;
	script.state["SDLK_DOWN"] = (int)SDLK_DOWN;
	script.state["SDLK_LEFT"] = (int)SDLK_LEFT;
	script.state["SDLK_SPACE"] = (int)SDLK_SPACE;
	script.state["SDLK_w"] = (int)SDLK_w;
	script.state["SDLK_a"] = (int)SDLK_a;
	script.state["SDLK_s"] = (int)SDLK_s;
	script.state["SDLK_d"] = (int)SDLK_d;
	script.state["SDLK_e"] = (int)SDLK_e;
	script.state["SDLK_q"] = (int)SDLK_q;
	script.state["SDLK_1"] = (int)SDLK_1;
	script.state["SDLK_2"] = (int)SDLK_2;
	script.state["SDLK_3"] = (int)SDLK_3;
	script.state["SDLK_4"] = (int)SDLK_4;
	script.state["SDLK_5"] = (int)SDLK_5;
	script.state["SDLK_6"] = (int)SDLK_6;

	script.state["MOUSE_BUTTON_LEFT"] = static_cast<int>(MouseButton::LEFT);
	script.state["MOUSE_BUTTON_MIDDLE"] = static_cast<int>(MouseButton::MIDDLE);
	script.state["MOUSE_BUTTON_RIGHT"] = static_cast<int>(MouseButton::RIGHT);

	EventDelegate mouseEventDelegate([&script](const EventData& eventData) {
		MouseEventData data = dynamic_cast<const MouseEventData&>(eventData);

		if (data.action == MouseAction::CLICK_UP) {
			script.invoke("onMouseUp", (double)data.x, (double)data.y, static_cast<int>(data.button));
		}
		else if (data.action == MouseAction::CLICK_DOWN) {
			script.invoke("onMouseDown", (double)data.x, (double)data.y, static_cast<int>(data.button));
		}
		return false;
		});

	EventListenerDelegate mouseEventListener(mouseEventDelegate);
	EventManager::getInstance().addDelegate(mouseEventListener, EventType::MOUSE_EVENT);

	script.deleters.push_back([mouseEventListener]() {
		EventManager::getInstance().removeDelegate(mouseEventListener, EventType::MOUSE_EVENT);
		});

	EventDelegate keyEventDelegate([&script](const EventData& eventData) {
		KeyEventData data = dynamic_cast<const KeyEventData&>(eventData);

		if (data.action == KeyAction::UP) {
			script.invoke("onKeyUp", (int)data.key, data.ctrl, data.shft);
		}
		else {
			script.invoke("onKeyDown", (int)data.key, data.ctrl, data.shft);
		}
		return false;
		});

	EventListenerDelegate keyEventListener(keyEventDelegate);
	EventManager::getInstance().addDelegate(keyEventListener, EventType::KEY_EVENT);

	script.deleters.push_back([keyEventListener]() {
		EventManager::getInstance().removeDelegate(keyEventListener, EventType::KEY_EVENT);
		});

	EntitySystemPtr entitySystem = systemManager->getSystemByType<EntitySystem>(SystemType::ENTITY);
	auto entity = entitySystem->getEntityById(entityId);
	if (entity->getComponents().find(ComponentType::PHYSICS_COMPONENT) == entity->getComponents().end()) {
		applyPhysics(systemManager, entityId, -1, -1, 1, 1);
	}

	applyInput(systemManager, entityId, Input::ON_MOUSE_ENTER, function<bool(EventPtr)>([&script](EventPtr evt) {
		script.invoke("onMouseEnterEntity");
		return false;
		}));

	applyInput(systemManager, entityId, Input::ON_MOUSE_EXIT, function<bool(EventPtr)>([&script](EventPtr evt) {
		script.invoke("onMouseExitEntity");
		return false;
		}));

	applyInput(systemManager, entityId, Input::ON_CLICK, function<bool(EventPtr)>([&script](EventPtr evt) {
		script.invoke("onClickEntity", static_cast<int>(evt->mouseEvent->button));
		return false;
		}));

	applyInput(systemManager, entityId, Input::ON_DRAG, function<bool(EventPtr)>([&script](EventPtr evt) {
		script.invoke("onDragEntity", static_cast<int>(evt->mouseEvent->button));
		return false;
		}));
}

void LuaScriptFactory::registerMouseMove(LuaScript& script) {
	script.state["enableMouseMove"] = [this, &script](int entityId) {
		applyInput(systemManager, entityId, Input::ON_MOUSE_MOVE, function<bool(EventPtr)>([&script](EventPtr evt) {
			int x = (int)std::roundf(evt->mouseEvent->position->x);
			int y = (int)std::roundf(evt->mouseEvent->position->y);
			script.invoke("onMouseMove", x, y, static_cast<int>(evt->mouseEvent->button));
			return false;
			}));
	};
}

void LuaScriptFactory::registerScript(LuaScript& script) {

	script.state["setScript"] = [this](int entityId, string path) {
		applyScript(systemManager, entityId, path);
	};
}

void LuaScriptFactory::registerUi(LuaScript& script) {
	script.state["createLabel"] = [this](string text, int fontSize, int x, int y) -> int {
		EntityPtr entity = widgetFactory->createLabel(text, fontSize, x, y);

		return entity->id;
	};

	script.state["setLabelZOrder"] = [this](int entityId, int order) {
		EntitySystemPtr entitySystem = systemManager->getSystemByType<EntitySystem>(SystemType::ENTITY);
		EntityPtr entity = entitySystem->getEntityById(entityId);

		if (entity->getComponents().find(ComponentType::LABEL_COMPONENT) != entity->getComponents().end()) {
			LabelComponentPtr drawableComponent = entity->getComponentByType<LabelComponent>(ComponentType::LABEL_COMPONENT);

			drawableComponent->setZOrder(order);
		}
	};

	script.state["setUI"] = [this](int entityId, bool ui) {
		EntitySystemPtr entitySystem = systemManager->getSystemByType<EntitySystem>(SystemType::ENTITY);
		EntityPtr entity = entitySystem->getEntityById(entityId);

		if (entity->getComponents().find(ComponentType::PROGRESS_COMPONENT) != entity->getComponents().end()) {
			ProgressComponentPtr progressComponent = entity->getComponentByType<ProgressComponent>(ComponentType::PROGRESS_COMPONENT);
			progressComponent->progressBar->isUi = ui;
		}
	};

	script.state["setLabelText"] = [this](int entityId, string text, int fontSize, int r, int g, int b) {
		applyLabel(systemManager, entityId, text, widgetFactory->getUIConfig()->fontTag, fontSize, r, g, b, 255);
	};

	script.state["createProgress"] = [this](int x, int y, int w, int h, int progressMax, int currentProgress) -> int {
		EntityPtr entity = widgetFactory->createProgressBar(x, y, w, h, progressMax, currentProgress);

		return entity->id;
	};

	script.state["setProgress"] = [this](int entityId, int progress, int maxProgress) {
		EntitySystemPtr entitySystem = systemManager->getSystemByType<EntitySystem>(SystemType::ENTITY);
		EntityPtr entity = entitySystem->getEntityById(entityId);

		ProgressComponentPtr progressComponent = entity->getComponentByType<ProgressComponent>(ComponentType::PROGRESS_COMPONENT);
		progressComponent->setProgress(progress, maxProgress);
	};

	script.state["createPanel"] = [this](int x, int y, int w, int h) -> int {
		EntityPtr entity = widgetFactory->createPanel(x, y, w, h);

		return entity->id;
	};
}

void LuaScriptFactory::registerAsset(LuaScript& script) {
	script.state["loadTexture"] = [this](string path, string tag) {
		GraphicsSystemPtr graphicsSystem = systemManager->getSystemByType<GraphicsSystem>(SystemType::GRAPHICS);

		graphicsSystem->addTexture(path, tag);
	};
	script.state["loadFont"] = [this](string path, string tag, int fontsize) {
		GraphicsSystemPtr graphicsSystem = systemManager->getSystemByType<GraphicsSystem>(SystemType::GRAPHICS);

		graphicsSystem->addFont(path, tag, fontsize);
	};
}


void LuaScriptFactory::registerCamera(LuaScript& script) {
	script.state["moveCameraBy"] = [this](int dx, int dy) {
		GraphicsSystemPtr graphicsSystem = systemManager->getSystemByType<GraphicsSystem>(SystemType::GRAPHICS);

		CameraPtr camera = makeShared(graphicsSystem->getCamera());
		*(camera->position) += Vector2f(dx, dy);
	};
	script.state["setCameraPosition"] = [this](int dx, int dy) {
		GraphicsSystemPtr graphicsSystem = systemManager->getSystemByType<GraphicsSystem>(SystemType::GRAPHICS);

		CameraPtr camera = makeShared(graphicsSystem->getCamera());
		camera->position->set(Vector2f(dx - graphicsSystem->getGraphicsConfig()->mWidth / 2.0f, dy - graphicsSystem->getGraphicsConfig()->mHeight / 2.0f));
	};


	script.state["getCameraPosition"] = [this]() -> LuaFriendlyVector2f& {
		GraphicsSystemPtr graphicsSystem = systemManager->getSystemByType<GraphicsSystem>(SystemType::GRAPHICS);

		CameraPtr camera = makeShared(graphicsSystem->getCamera());

		LuaFriendlyVector2f* vec = GCC_NEW LuaFriendlyVector2f(camera->position->x, camera->position->y);

		return *vec;
	};
}

void LuaScriptFactory::registerWorld(LuaScript& script) {
	script.state["Path"].SetClass<Path>(
		"size", &Path::size,
		"getX", &Path::getX,
		"getY", &Path::getY);

	script.state["loadWorld"] = [this](const string& path) {
		LoadWorldData* eventData = GCC_NEW LoadWorldData(SDL_GetTicks(), path);
		EventManager::getInstance().pushEvent(eventData);
	};

	script.state["getTileCoordinatesAtPoint"] = [this](double x, double y) -> LuaFriendlyVector2f& {
		WorldSystemPtr worldSystem = systemManager->getSystemByType<WorldSystem>(SystemType::WORLD);

		WorldPtr world = worldSystem->getWorld();

		if (x < 0.0 || x > world->getMap()->getMapWidthPixels() || y < 0.0 || y > world->getMap()->getMapHeightPixels()) {
			Vector2f coord(-1, -1);
			LuaFriendlyVector2f* vec = GCC_NEW LuaFriendlyVector2f(coord);

			return *vec;
		}

		Vector2f point(x, y);
		CellPtr cell = world->getCellAtPoint(point);

		Vector2f coord(cell->x, cell->y);
		LuaFriendlyVector2f* vec = GCC_NEW LuaFriendlyVector2f(coord);

		return *vec;
	};

	script.state["getTileLocation"] = [this](double x, double y) -> LuaFriendlyVector2f& {
		WorldSystemPtr worldSystem = systemManager->getSystemByType<WorldSystem>(SystemType::WORLD);

		WorldPtr world = worldSystem->getWorld();

		if (x < 0.0 || x > world->getMap()->getMapWidth() || y < 0.0 || y > world->getMap()->getMapHeight()) {
			Vector2f coord(-1, -1);
			LuaFriendlyVector2f* vec = GCC_NEW LuaFriendlyVector2f(coord);

			return *vec;
		}

		Vector2f point(x * world->getMap()->getTileWidth() + (world->getMap()->getTileWidth() / 2.0f), y * world->getMap()->getTileHeight() + (world->getMap()->getTileHeight() / 2.0f));
		LuaFriendlyVector2f* vec = GCC_NEW LuaFriendlyVector2f(point);

		return *vec;
	};

	script.state["pushTexture"] = [this](int x, int y, const string& path, double tx, double ty, double tw, double th) {
		WorldSystemPtr worldSystem = systemManager->getSystemByType<WorldSystem>(SystemType::WORLD);

		WorldPtr world = worldSystem->getWorld();

		if (x >= 0 && x < world->getMap()->getMapWidth() && y >= 0 && y < world->getMap()->getMapHeight()) {
			GraphicsSystemPtr graphicsSystem = systemManager->getSystemByType<GraphicsSystem>(SystemType::GRAPHICS);
			graphicsSystem->addTexture(path, path);

			EntityPtr grid = world->getGridWithTileAt(x, y);
			GridComponentPtr gridComponent = grid->getComponentByType<GridComponent>(ComponentType::GRID_COMPONENT);
			TexturePtr texture = std::make_shared<Texture>(path, tx, ty, tw, th);
			gridComponent->pushTexture(x, y, texture);
		}
	};

	script.state["pushTextureAtPoint"] = [this](int x, int y, const string& path, double tx, double ty, double tw, double th) {
		WorldSystemPtr worldSystem = systemManager->getSystemByType<WorldSystem>(SystemType::WORLD);

		WorldPtr world = worldSystem->getWorld();

		if (x >= 0 && x < world->getMap()->getMapWidth() && y >= 0 && y < world->getMap()->getMapHeight()) {
			GraphicsSystemPtr graphicsSystem = systemManager->getSystemByType<GraphicsSystem>(SystemType::GRAPHICS);
			graphicsSystem->addTexture(path, path);

			EntityPtr grid = world->getGridWithTileAt(x, y);
			GridComponentPtr gridComponent = grid->getComponentByType<GridComponent>(ComponentType::GRID_COMPONENT);
			TexturePtr texture = std::make_shared<Texture>(path, tx, ty, tw, th);
			gridComponent->pushTextureAtPoint(Vector2f(x, y), texture);
		}
	};

	script.state["popTexture"] = [this](int x, int y) {
		WorldSystemPtr worldSystem = systemManager->getSystemByType<WorldSystem>(SystemType::WORLD);

		WorldPtr world = worldSystem->getWorld();

		if (x < 0 || x > world->getMap()->getMapWidth() || y < 0 || y > world->getMap()->getMapHeight()) {
			return;
		}

		EntityPtr grid = world->getGridWithTileAt(x, y);
		GridComponentPtr gridComponent = grid->getComponentByType<GridComponent>(ComponentType::GRID_COMPONENT);
		gridComponent->popTexture(x, y);
	};

	script.state["getPath"] = [this](double sx, double sy, double ex, double ey) -> Path& {
		WorldSystemPtr worldSystem = systemManager->getSystemByType<WorldSystem>(SystemType::WORLD);

		WorldPtr world = worldSystem->getWorld();

		Path* path = world->buildPath(sx, sy, ex, ey);

		return *path;
	};
}

void LuaScriptFactory::registerSound(LuaScript& script) {
	script.state["loadSound"] = [this](const string& path, const string& tag) {
		SoundSystemPtr soundSystem = systemManager->getSystemByType<SoundSystem>(SystemType::SOUND);

		soundSystem->loadSound(path, tag, SoundType::SOUND);
	};

	script.state["loadMusic"] = [this](const string& path, const string& tag) {
		SoundSystemPtr soundSystem = systemManager->getSystemByType<SoundSystem>(SystemType::SOUND);

		soundSystem->loadSound(path, tag, SoundType::MUSIC);
	};

	script.state["playSound"] = [this](const string& tag, int channel, int loop) {
		SoundSystemPtr soundSystem = systemManager->getSystemByType<SoundSystem>(SystemType::SOUND);

		soundSystem->playSound(tag, channel, loop);
	};

	script.state["playMusic"] = [this](const string& tag) {
		SoundSystemPtr soundSystem = systemManager->getSystemByType<SoundSystem>(SystemType::SOUND);

		soundSystem->playMusic(tag);
	};

	script.state["stopMusic"] = [this](const string& tag) {
		SoundSystemPtr soundSystem = systemManager->getSystemByType<SoundSystem>(SystemType::SOUND);

		soundSystem->stopMusic(tag);
	};

	script.state["stopSound"] = [this](const string& tag, int channel) {
		SoundSystemPtr soundSystem = systemManager->getSystemByType<SoundSystem>(SystemType::SOUND);

		soundSystem->stopSound(tag, channel);
	};
}