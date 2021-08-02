#include"ScriptFactory.h"

LuaScriptPtr LuaScriptFactory::create(const string& scriptPath, unsigned long entityId) {
	LuaScriptPtr script(GCC_NEW LuaScript(scriptPath));

	this->initialize(script, entityId);

	return script;
}

void LuaScriptFactory::initialize(LuaScriptPtr script, unsigned long entityId) {

	script->state["entityId"] = (int)entityId;

	int DRAWABLE = script->state["registrar"]["DRAWABLE"];
	int ENTITY = script->state["registrar"]["ENTITY"];
	int FACTORY = script->state["registrar"]["FACTORY"];
	int PHYSICS = script->state["registrar"]["PHYSICS"];
	int ANIMATION = script->state["registrar"]["ANIMATION"];
	int INPUT = script->state["registrar"]["INPUT"];
	int SCRIPT = script->state["registrar"]["SCRIPT"];
	int UI = script->state["registrar"]["UI"];
	int ASSET = script->state["registrar"]["ASSET"];
	int CAMERA = script->state["registrar"]["CAMERA"];
	int MOUSE_MOVE = script->state["registrar"]["MOUSE_MOVE"];
	int MAP = script->state["registrar"]["MAP"];

	script->state["IntList"].SetClass<LuaFriendlyIntVector>(
		"size", &LuaFriendlyIntVector::size,
		"at", &LuaFriendlyIntVector::at,
		"push", &LuaFriendlyIntVector::push);

	script->state["StringList"].SetClass<LuaFriendlyStringVector>(
		"size", &LuaFriendlyStringVector::size,
		"at", &LuaFriendlyStringVector::at,
		"push", &LuaFriendlyStringVector::push);

	script->state["StringMap"].SetClass<LuaFriendlyStringMap>(
		"size", &LuaFriendlyStringMap::size,
		"at", &LuaFriendlyStringMap::at,
		"put", &LuaFriendlyStringMap::put);

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
	if (MAP) {
		this->registerMap(script);
	}

	auto output = script->invoke("setup");
}

void LuaScriptFactory::clean() {
	//this->toDelete.clear();
}

void LuaScriptFactory::registerGeneral(LuaScriptPtr script) {
	script->state["sendMessage"] = [this](int entityId, string message, string value) -> string {
		EntitySystemPtr entitySystem = mSystemManager->getSystemByType<EntitySystem>(SystemType::ENTITY);
		EntityPtr entity = makeShared<Entity>(entitySystem->getEntityById(entityId));

		LuaScriptComponentPtr scriptComponent = entity->getComponentByType<LuaScriptComponent>(ComponentType::LUA_SCRIPT_COMPONENT);

		return scriptComponent->script->invoke("onMessage", message, value);
	};

	script->state["loadData"] = [this](string path) {
		SystemManagerPtr systemManager = makeShared<SystemManager>(mSystemManager);
		DataSystemPtr dataSystem = mSystemManager->getSystemByType<DataSystem>(SystemType::DATA);
		dataSystem->load(path);
	};

	script->state["hasData"] = [this](string path, string key) -> bool {
		DataSystemPtr dataSystem = mSystemManager->getSystemByType<DataSystem>(SystemType::DATA);
		return dataSystem->hasData(path, key);
	};

	script->state["getData"] = [this](string path, string key) -> string {
		DataSystemPtr dataSystem = mSystemManager->getSystemByType<DataSystem>(SystemType::DATA);
		return dataSystem->getData(path, key);
	};

	script->state["putData"] = [this](string path, string key, string val) {
		DataSystemPtr dataSystem = mSystemManager->getSystemByType<DataSystem>(SystemType::DATA);
		dataSystem->putData(path, key, val);
	};

	script->state["saveData"] = [this](string path) {
		DataSystemPtr dataSystem = mSystemManager->getSystemByType<DataSystem>(SystemType::DATA);
		dataSystem->save(path);
	};

	script->state["closeData"] = [this](string path) {
		DataSystemPtr dataSystem = mSystemManager->getSystemByType<DataSystem>(SystemType::DATA);
		dataSystem->close(path);
	};
}

void LuaScriptFactory::registerFactory(LuaScriptPtr script) {
	script->state["createDefault"] = [this](int x, int y, int w, int h, int r, int g, int b, int a) -> int {
		EntityPtr entity = mEntityFactory->createDefault(x, y, w, h, r, g, b, a);
		return (int)entity->id;
	};

	script->state["createTextured"] = [this](string tag, int x, int y, int width, int height, int tx, int ty, int w, int h, bool collidable) -> int {
		GraphicsSystemPtr graphicsSystem = mSystemManager->getSystemByType<GraphicsSystem>(SystemType::GRAPHICS);
		graphicsSystem->addTexture(tag, tag);

		EntityPtr entity = mEntityFactory->createTexturedEntity(tag, x, y, width, height, tx, ty, w, h, collidable);
		return (int)entity->id;
	};

	script->state["createAnimated"] = [this](string path, int x, int y, int width, int height) -> int {
		EntityPtr entity = mEntityFactory->createAnimatedEntity(path, x, y, width, height);
		return (int)entity->id;
	};

	script->state["createPhysics"] = [this](int x, int y, int width, int height) -> int {
		EntityPtr entity = mEntityFactory->createPhysicsEntity(x, y, width, height);
		return (int)entity->id;
	};

	script->state["createSerialized"] = [this](string path) -> int {
		EntityPtr entity = mEntityFactory->createFromSerialization(path);

		return (int)entity->id;
	};
}

void LuaScriptFactory::registerEntity(LuaScriptPtr script) {
	script->state["destroyEntity"] = [this](int entityId) {
		EntitySystemPtr entitySystem = mSystemManager->getSystemByType<EntitySystem>(SystemType::ENTITY);
		entitySystem->deregisterEntity(entityId);
	};


	script->state["setChild"] = [this](int parent, int child) {
		EntitySystemPtr entitySystem = mSystemManager->getSystemByType<EntitySystem>(SystemType::ENTITY);
		EntityPtr childEntity = makeShared<Entity>(entitySystem->getEntityById(child));

		EntityPtr parentEntity = makeShared<Entity>(entitySystem->getEntityById(parent));
		parentEntity->children.push_back(childEntity);
		childEntity->parent = parentEntity;
	};
}

void LuaScriptFactory::registerPhysics(LuaScriptPtr script) {

	script->state["Vector2f"].SetClass<LuaFriendlyVector2f, double, double>(
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
		"subtract", &LuaFriendlyVector2f::subtract);

	script->state["setVelocity"] = [this](int entityId, double x, double y) {
		EntitySystemPtr entitySystem = mSystemManager->getSystemByType<EntitySystem>(SystemType::ENTITY);

		EntityPtr entity = entitySystem->getEntityById(entityId);

		PhysicsComponentPtr physicsComponent = entity->getComponentByType<PhysicsComponent>(ComponentType::PHYSICS_COMPONENT);
		Vector2f velocity(x, y);
		velocity.normalize();
		physicsComponent->setVelocity(velocity);
	};

	script->state["addVelocity"] = [this](int entityId, double x, double y) {
		EntitySystemPtr entitySystem = mSystemManager->getSystemByType<EntitySystem>(SystemType::ENTITY);

		EntityPtr entity = entitySystem->getEntityById(entityId);

		PhysicsComponentPtr physicsComponent = entity->getComponentByType<PhysicsComponent>(ComponentType::PHYSICS_COMPONENT);
		physicsComponent->setVelocity(physicsComponent->getVelocity() + Vector2f(x, y));
	};

	script->state["setSpeed"] = [this](int entityId, double speed) {
		EntitySystemPtr entitySystem = mSystemManager->getSystemByType<EntitySystem>(SystemType::ENTITY);

		EntityPtr entity = entitySystem->getEntityById(entityId);

		PhysicsComponentPtr physicsComponent = entity->getComponentByType<PhysicsComponent>(ComponentType::PHYSICS_COMPONENT);
		physicsComponent->setSpeed(speed);
	};

	script->state["getSpeed"] = [this](int entityId) -> double {
		EntitySystemPtr entitySystem = mSystemManager->getSystemByType<EntitySystem>(SystemType::ENTITY);

		EntityPtr entity = entitySystem->getEntityById(entityId);

		PhysicsComponentPtr physicsComponent = entity->getComponentByType<PhysicsComponent>(ComponentType::PHYSICS_COMPONENT);
		return (double)physicsComponent->getSpeed();
	};

	script->state["getPosition"] = [this](int entityId) -> LuaFriendlyVector2f& {
		EntitySystemPtr entitySystem = mSystemManager->getSystemByType<EntitySystem>(SystemType::ENTITY);

		EntityPtr entity = entitySystem->getEntityById(entityId);

		PhysicsComponentPtr physicsComponent = entity->getComponentByType<PhysicsComponent>(ComponentType::PHYSICS_COMPONENT);
		const Vector2f& position = physicsComponent->getPosition();
		LuaFriendlyVector2f* vec = GCC_NEW LuaFriendlyVector2f(position);

		toDelete.push_back(VoidPtr(vec));

		return *vec;
	};

	script->state["setPosition"] = [this](int entityId, double x, double y) {
		EntitySystemPtr entitySystem = mSystemManager->getSystemByType<EntitySystem>(SystemType::ENTITY);

		EntityPtr entity = entitySystem->getEntityById(entityId);

		PhysicsComponentPtr physicsComponent = entity->getComponentByType<PhysicsComponent>(ComponentType::PHYSICS_COMPONENT);
		Vector2f position(x, y);
		physicsComponent->setPosition(position);
	};

	script->state["getVelocity"] = [this](int entityId) -> LuaFriendlyVector2f& {
		EntitySystemPtr entitySystem = mSystemManager->getSystemByType<EntitySystem>(SystemType::ENTITY);

		EntityPtr entity = entitySystem->getEntityById(entityId);

		PhysicsComponentPtr physicsComponent = entity->getComponentByType<PhysicsComponent>(ComponentType::PHYSICS_COMPONENT);
		auto vec = *(GCC_NEW LuaFriendlyVector2f(const_cast<Vector2f&>(physicsComponent->getVelocity())));

		toDelete.push_back(VoidPtr(&vec));

		return vec;
	};

	script->state["checkCollisionsArea"] = [this](int x0, int y0, int x1, int y1) -> LuaFriendlyIntVector& {
		PhysicsSystemPtr physicsSystem = mSystemManager->getSystemByType<PhysicsSystem>(SystemType::PHYSICS);

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

		toDelete.push_back(colliderShape);
		toDelete.push_back(collider);
		toDelete.push_back(body);

		vector<WeakBodyPtr> bodies;
		physicsSystem->quadTree->getCollidingBodies(body, bodies);

		LuaFriendlyIntVector* ids = GCC_NEW LuaFriendlyIntVector();
		for (auto& bodyPtr : bodies) {
			BodyPtr b = makeShared(bodyPtr);
			ids->push(b->id);
		}

		toDelete.push_back(VoidPtr(ids));

		return *ids;
	};

	script->state["checkCollisions"] = [this](int entityId) -> LuaFriendlyIntVector& {
		PhysicsSystemPtr physicsSystem = mSystemManager->getSystemByType<PhysicsSystem>(SystemType::PHYSICS);

		BodyPtr body = makeShared(physicsSystem->getBody(entityId));
		LuaFriendlyIntVector* ids = GCC_NEW LuaFriendlyIntVector();
		if (!body->isCollidable()) {
			return *ids;
		}

		toDelete.push_back(VoidPtr(ids));

		vector<WeakBodyPtr> bodies;
		physicsSystem->quadTree->getCollidingBodies(body, bodies);

		for (auto& bodyPtr : bodies) {
			BodyPtr b = makeShared(bodyPtr);
			ids->push(b->id);
		}

		return *ids;
	};

	script->state["setAABBCollision"] = [this](int entityId, int width, int height) {
		EntitySystemPtr entitySystem = mSystemManager->getSystemByType<EntitySystem>(SystemType::ENTITY);

		EntityPtr entity = entitySystem->getEntityById(entityId);

		PhysicsComponentPtr physicsComponent = entity->getComponentByType<PhysicsComponent>(ComponentType::PHYSICS_COMPONENT);
		BodyPtr body = makeShared(physicsComponent->getBody());
		Vector2fPtr pos = std::make_shared<Vector2f>(body->position->x, body->position->y);
		body->setCollider(ColliderPtr(GCC_NEW Collider(GCC_NEW AABBColliderShape(pos, width, height))));
	};

	script->state["setCircleCollision"] = [this](int entityId, int radius) {
		EntitySystemPtr entitySystem = mSystemManager->getSystemByType<EntitySystem>(SystemType::ENTITY);

		EntityPtr entity = entitySystem->getEntityById(entityId);

		PhysicsComponentPtr physicsComponent = entity->getComponentByType<PhysicsComponent>(ComponentType::PHYSICS_COMPONENT);
		BodyPtr body = makeShared(physicsComponent->getBody());
		Vector2fPtr pos = std::make_shared<Vector2f>(body->position->x, body->position->y);
		body->setCollider(ColliderPtr(GCC_NEW Collider(GCC_NEW CircleColliderShape(pos, radius))));
	};

	script->state["setOBBCollision"] = [this](int entityId, int width, int height, int angle) {
		EntitySystemPtr entitySystem = mSystemManager->getSystemByType<EntitySystem>(SystemType::ENTITY);

		EntityPtr entity = entitySystem->getEntityById(entityId);

		PhysicsComponentPtr physicsComponent = entity->getComponentByType<PhysicsComponent>(ComponentType::PHYSICS_COMPONENT);
		BodyPtr body = makeShared(physicsComponent->getBody());
		Vector2fPtr pos = std::make_shared<Vector2f>(body->position->x, body->position->y);
		body->setCollider(ColliderPtr(GCC_NEW Collider(GCC_NEW OBBColliderShape(pos, width, height, angle))));
	};

	script->state["setTarget"] = [this](int entityId, double x, double y, double threshold) {
		EntitySystemPtr entitySystem = mSystemManager->getSystemByType<EntitySystem>(SystemType::ENTITY);

		EntityPtr entity = entitySystem->getEntityById(entityId);

		PhysicsComponentPtr physicsComponent = entity->getComponentByType<PhysicsComponent>(ComponentType::PHYSICS_COMPONENT);
		Vector2fPtr position(GCC_NEW Vector2f(x, y));
		TargetPtr target(GCC_NEW PositionTarget(position));
		target->setThreshold(threshold);

		physicsComponent->setTarget(target);
	};

	script->state["setTag"] = [this](int entityId, string tag) {
		EntitySystemPtr entitySystem = mSystemManager->getSystemByType<EntitySystem>(SystemType::ENTITY);

		EntityPtr entity = entitySystem->getEntityById(entityId);

		PhysicsComponentPtr physicsComponent = entity->getComponentByType<PhysicsComponent>(ComponentType::PHYSICS_COMPONENT);
		physicsComponent->setTag(tag);
	};

	script->state["getTag"] = [this](int entityId) {
		EntitySystemPtr entitySystem = mSystemManager->getSystemByType<EntitySystem>(SystemType::ENTITY);

		EntityPtr entity = entitySystem->getEntityById(entityId);

		PhysicsComponentPtr physicsComponent = entity->getComponentByType<PhysicsComponent>(ComponentType::PHYSICS_COMPONENT);
		return physicsComponent->getTag();
	};

	EventDelegate destroyEntityDelegate([script](const EventData& eventData) {
		EntityCollisionEventData data = dynamic_cast<const EntityCollisionEventData&>(eventData);

		int id = script->state["entityId"];
		if (id == (int)data.getCollidedEntityId()) {
			script->invoke("onCollision", static_cast<int>((int)data.getColliderEntityId()));
		}
		else if (id == (int)data.getColliderEntityId()) {
			script->invoke("onCollision", static_cast<int>((int)data.getCollidedEntityId()));
		}
		});

	EventListenerDelegate destroyEntityListener(destroyEntityDelegate);
	EventManager::getInstance().addDelegate(destroyEntityListener, EventType::ENTITY_COLLISION_EVENT);

	script->deleters.push_back([destroyEntityListener]() {
		EventManager::getInstance().removeDelegate(destroyEntityListener, EventType::ENTITY_COLLISION_EVENT);
		});
}

void LuaScriptFactory::registerDrawable(LuaScriptPtr script) {
	script->state["setTexture"] = [this](string tag, int entityId, double width, double height, double tx, double ty, double w, double h) {
		GraphicsSystemPtr graphicsSystem = mSystemManager->getSystemByType<GraphicsSystem>(SystemType::GRAPHICS);
		graphicsSystem->addTexture(tag, tag);

		applyDrawable(mSystemManager, entityId, tag, width, height, tx, ty, w, h);
	};

	script->state["drawSquare"] = [this](int x0, int y0, int x1, int y1, int r, int g, int b, int a) {
		GraphicsSystemPtr graphicsSystem = mSystemManager->getSystemByType<GraphicsSystem>(SystemType::GRAPHICS);
	};


	script->state["getZOrder"] = [this](int entityId) -> int {
		EntitySystemPtr entitySystem = mSystemManager->getSystemByType<EntitySystem>(SystemType::ENTITY);
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

	script->state["setZOrder"] = [this](int entityId, int order) {
		EntitySystemPtr entitySystem = mSystemManager->getSystemByType<EntitySystem>(SystemType::ENTITY);
		EntityPtr entity = entitySystem->getEntityById(entityId);

		if (entity->getComponents().find(ComponentType::DRAWABLE_COMPONENT) != entity->getComponents().end()) {
			DrawableComponentPtr drawableComponent = entity->getComponentByType<DrawableComponent>(ComponentType::DRAWABLE_COMPONENT);

			drawableComponent->setZOrder(order);
		}
		else if (entity->getComponents().find(ComponentType::ANIMATION_COMPONENT) != entity->getComponents().end()) {
			AnimationComponentPtr animationComponent = entity->getComponentByType<AnimationComponent>(ComponentType::ANIMATION_COMPONENT);
			animationComponent->setZOrder(order);
		}
	};

	script->state["setSize"] = [this](int entityId, double w, double h) {
		EntitySystemPtr entitySystem = mSystemManager->getSystemByType<EntitySystem>(SystemType::ENTITY);
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

	script->state["setAngle"] = [this](int entityId, double angle) {
		EntitySystemPtr entitySystem = mSystemManager->getSystemByType<EntitySystem>(SystemType::ENTITY);
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
			BodyPtr body = makeShared(physicsComponent->getBody());
			if (body->isCollidable() && body->collider->colliderShape->colliderType() == ColliderType::OBB) {
				OBBColliderShapePtr obb = dynamic_pointer_cast<OBBColliderShape>(body->collider->colliderShape);
				obb->setAngle(angle);
			}
		}
	};

	script->state["getAngle"] = [this](int entityId) {
		EntitySystemPtr entitySystem = mSystemManager->getSystemByType<EntitySystem>(SystemType::ENTITY);
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

	script->state["setColor"] = [this](int entityId, int r, int g, int b, int a) {
		EntitySystemPtr entitySystem = mSystemManager->getSystemByType<EntitySystem>(SystemType::ENTITY);
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

	script->state["addParticle"] = [this](int entityId, int particleLifeMillis, int spreadDist, int gravX, int gravY, int partW, int partH) {
		EntitySystemPtr entitySystem = mSystemManager->getSystemByType<EntitySystem>(SystemType::ENTITY);
		EntityPtr entity = entitySystem->getEntityById(entityId);

		GraphicsSystemPtr graphicsSystem = mSystemManager->getSystemByType<GraphicsSystem>(SystemType::GRAPHICS);
		graphicsSystem->addTexture("Assets/HackNSlasher/Visual FX/Looping Fire/Fireball 16x16.png", "Assets/HackNSlasher/Visual FX/Looping Fire/Fireball 16x16.png");
		ParticleCloudPtr particleCloud = std::make_shared<ParticleCloud>();

		particleCloud->particleLifeMillis = particleLifeMillis;
		particleCloud->spreadDist = spreadDist;
		particleCloud->gravity = std::make_shared<Vector2f>((float)gravX, (float)gravY);
		particleCloud->fade = 0.95f;

		particleCloud->particleTextures.push_back(std::make_shared<Texture>("Assets/HackNSlasher/Visual FX/Looping Fire/Fireball 16x16.png", 0, 0, 16, 16));
		particleCloud->particleTextures.push_back(std::make_shared<Texture>("Assets/HackNSlasher/Visual FX/Looping Fire/Fireball 16x16.png", 16, 0, 16, 16));
		particleCloud->particleTextures.push_back(std::make_shared<Texture>("Assets/HackNSlasher/Visual FX/Looping Fire/Fireball 16x16.png", 32, 0, 16, 16));
		particleCloud->particleTextures.push_back(std::make_shared<Texture>("Assets/HackNSlasher/Visual FX/Looping Fire/Fireball 16x16.png", 48, 0, 16, 16));
		particleCloud->particleTextures.push_back(std::make_shared<Texture>("Assets/HackNSlasher/Visual FX/Looping Fire/Fireball 16x16.png", 64, 0, 16, 16));

		ParticleCloudDrawablePtr particleCloudDrawable = std::make_shared<ParticleCloudDrawable>(particleCloud);
		particleCloudDrawable->setSize(partW, partH);
		ParticleSystemPtr particleSystem = mSystemManager->getSystemByType<ParticleSystem>(SystemType::PARTICLE);
		particleSystem->registerParticleEmitter(entity->id, particleCloudDrawable);

		DrawableComponentPtr drawableComponent = std::make_shared<DrawableComponent>(entity->id, particleCloudDrawable);
		graphicsSystem->registerDrawable(entity->id, particleCloudDrawable);
		drawableComponent->setZOrder(10);
		entity->addComponent(drawableComponent);
	};
}

void LuaScriptFactory::registerAnimation(LuaScriptPtr script) {
	script->state["attachAnimationSet"] = [this](int entityId, string path) {
		applyAnimation(mSystemManager, entityId, path);
	};

	script->state["attachAsepriteAnimationSet"] = [this](int entityId, string path) {
		AssetSystemPtr assetSystem = mSystemManager->getSystemByType<AssetSystem>(SystemType::ASSET);
		AssetPtr asset = assetSystem->getAsset(path);
		if (asset == nullptr) {
			AnimationSetPtr animationSet = loadAsepriteAnimation(path);
			asset = std::make_shared<Asset>(VoidPtr(animationSet), path, path);
		}
		applyAnimation(mSystemManager, entityId, asset->getAsset<AnimationSet>());
	};

	script->state["setAnimation"] = [this](int entityId, string animationName) {
		EntitySystemPtr entitySystem = mSystemManager->getSystemByType<EntitySystem>(SystemType::ENTITY);
		EntityPtr entity = entitySystem->getEntityById(entityId);

		AnimationComponentPtr animationComponent = entity->getComponentByType<AnimationComponent>(ComponentType::ANIMATION_COMPONENT);

		animationComponent->setAnimation(animationName);
	};

	script->state["stopAnimation"] = [this](int entityId) {
		EntitySystemPtr entitySystem = mSystemManager->getSystemByType<EntitySystem>(SystemType::ENTITY);
		EntityPtr entity = entitySystem->getEntityById(entityId);

		AnimationComponentPtr animationComponent = entity->getComponentByType<AnimationComponent>(ComponentType::ANIMATION_COMPONENT);

		animationComponent->stop();
	};

	script->state["playAnimation"] = [this](int entityId) {
		EntitySystemPtr entitySystem = mSystemManager->getSystemByType<EntitySystem>(SystemType::ENTITY);
		EntityPtr entity = entitySystem->getEntityById(entityId);

		AnimationComponentPtr animationComponent = entity->getComponentByType<AnimationComponent>(ComponentType::ANIMATION_COMPONENT);

		animationComponent->play();
	};

	script->state["loopAnimation"] = [this](int entityId) {
		EntitySystemPtr entitySystem = mSystemManager->getSystemByType<EntitySystem>(SystemType::ENTITY);
		EntityPtr entity = entitySystem->getEntityById(entityId);

		AnimationComponentPtr animationComponent = entity->getComponentByType<AnimationComponent>(ComponentType::ANIMATION_COMPONENT);

		animationComponent->loop();
	};

	script->state["isAnimationPlaying"] = [this](int entityId) {
		EntitySystemPtr entitySystem = mSystemManager->getSystemByType<EntitySystem>(SystemType::ENTITY);
		EntityPtr entity = entitySystem->getEntityById(entityId);

		AnimationComponentPtr animationComponent = entity->getComponentByType<AnimationComponent>(ComponentType::ANIMATION_COMPONENT);

		return animationComponent->getAnimationState() == AnimationState::PLAYING || animationComponent->getAnimationState() == AnimationState::LOOPING;
	};
}

void LuaScriptFactory::registerInput(LuaScriptPtr script, unsigned long entityId) {

	script->state["SDLK_SPACE"] = (int)SDLK_SPACE;
	script->state["SDLK_UP"] = (int)SDLK_UP;
	script->state["SDLK_RIGHT"] = (int)SDLK_RIGHT;
	script->state["SDLK_DOWN"] = (int)SDLK_DOWN;
	script->state["SDLK_LEFT"] = (int)SDLK_LEFT;
	script->state["SDLK_SPACE"] = (int)SDLK_SPACE;
	script->state["SDLK_w"] = (int)SDLK_w;
	script->state["SDLK_a"] = (int)SDLK_a;
	script->state["SDLK_s"] = (int)SDLK_s;
	script->state["SDLK_d"] = (int)SDLK_d;
	script->state["SDLK_1"] = (int)SDLK_1;
	script->state["SDLK_2"] = (int)SDLK_2;
	script->state["SDLK_3"] = (int)SDLK_3;
	script->state["SDLK_4"] = (int)SDLK_4;
	script->state["SDLK_5"] = (int)SDLK_5;
	script->state["SDLK_6"] = (int)SDLK_6;

	script->state["MOUSE_BUTTON_LEFT"] = static_cast<int>(MouseButton::LEFT);
	script->state["MOUSE_BUTTON_MIDDLE"] = static_cast<int>(MouseButton::MIDDLE);
	script->state["MOUSE_BUTTON_RIGHT"] = static_cast<int>(MouseButton::RIGHT);

	EventDelegate mouseEventDelegate([script](const EventData& eventData) {
		MouseEventData data = dynamic_cast<const MouseEventData&>(eventData);

		if (data.action == MouseAction::CLICK_UP) {
			script->invoke("onMouseUp", (double)data.x, (double)data.y, static_cast<int>(data.button));
		}
		else if (data.action == MouseAction::CLICK_DOWN) {
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

	EntitySystemPtr entitySystem = mSystemManager->getSystemByType<EntitySystem>(SystemType::ENTITY);
	auto entity = entitySystem->getEntityById(entityId);
	if (entity->getComponents().find(ComponentType::PHYSICS_COMPONENT) == entity->getComponents().end()) {
		applyPhysics(mSystemManager, entityId, -1, -1, 1, 1);
	}

	applyInput(mSystemManager, entityId, Input::ON_MOUSE_ENTER, function<bool(EventPtr)>([script](EventPtr evt) {
		script->invoke("onMouseEnterEntity");
		return false;
		}));

	applyInput(mSystemManager, entityId, Input::ON_MOUSE_EXIT, function<bool(EventPtr)>([script](EventPtr evt) {
		script->invoke("onMouseExitEntity");
		return false;
		}));

	applyInput(mSystemManager, entityId, Input::ON_CLICK, function<bool(EventPtr)>([script](EventPtr evt) {
		script->invoke("onClickEntity", static_cast<int>(evt->mouseEvent->button));
		return false;
		}));

	applyInput(mSystemManager, entityId, Input::ON_DRAG, function<bool(EventPtr)>([script](EventPtr evt) {
		script->invoke("onDragEntity", static_cast<int>(evt->mouseEvent->button));
		return false;
		}));
}

void LuaScriptFactory::registerMouseMove(LuaScriptPtr script) {
	script->state["enableMouseMove"] = [this, script](int entityId) {
		applyInput(mSystemManager, entityId, Input::ON_MOUSE_MOVE, function<bool(EventPtr)>([script](EventPtr evt) {
			int x = (int)std::roundf(evt->mouseEvent->position->x);
			int y = (int)std::roundf(evt->mouseEvent->position->y);
			script->invoke("onMouseMove", x, y, static_cast<int>(evt->mouseEvent->button));
			return false;
			}));
	};
}


void LuaScriptFactory::registerScript(LuaScriptPtr script) {

	script->state["setScript"] = [this](int entityId, string path) {
		applyScript(mSystemManager, entityId, path);
	};
}

void LuaScriptFactory::registerUi(LuaScriptPtr script) {

	script->state["setText"] = [this](int entityId, string text, string font, int r, int g, int b) {
		EntitySystemPtr entitySystem = mSystemManager->getSystemByType<EntitySystem>(SystemType::ENTITY);
		EntityPtr entity = entitySystem->getEntityById(entityId);

		PhysicsComponentPtr physicsComponent;
		if (entity->getComponents().find(ComponentType::PHYSICS_COMPONENT) == entity->getComponents().end()) {
			PhysicsSystemPtr physicsSystem = mSystemManager->getSystemByType<PhysicsSystem>(SystemType::PHYSICS);
			BodyPtr blockBody(GCC_NEW Body(entity->id, 0, 0, 0, 0));
			physicsSystem->registerBody(entity->id, blockBody);
			physicsComponent.reset(GCC_NEW PhysicsComponent(entity->id, blockBody));
			entity->addComponent(physicsComponent);
		}
		else {
			physicsComponent = entity->getComponentByType<PhysicsComponent>(ComponentType::PHYSICS_COMPONENT);
		}
		LabelComponentPtr labelComponent;
		if (entity->getComponents().find(ComponentType::LABEL_COMPONENT) == entity->getComponents().end()) {
			GraphicsSystemPtr graphicsSystem = mSystemManager->getSystemByType<GraphicsSystem>(SystemType::GRAPHICS);

			TextDrawablePtr drawable(GCC_NEW TextDrawable(text, font));
			drawable->setDrawDepth(100);
			drawable->setColor(r, g, b, 255);
			graphicsSystem->registerDrawable(entity->id, drawable);

			labelComponent.reset(GCC_NEW LabelComponent(entity->id));
			entity->addComponent(labelComponent);
		}
		else {
			labelComponent = entity->getComponentByType<LabelComponent>(ComponentType::LABEL_COMPONENT);
		}

		labelComponent->setText(text, mSystemManager);
	};

	script->state["setProgress"] = [this](int entityId, string location, int progress, int maxProgress) {
		EntitySystemPtr entitySystem = mSystemManager->getSystemByType<EntitySystem>(SystemType::ENTITY);
		EntityPtr entity = entitySystem->getEntityById(entityId);

		EntityPtr progressEntity{ nullptr };
		auto it = entity->children.begin();
		while (it != entity->children.end()) {
			EntityPtr temp = *it;
			if (temp->getComponents().find(ComponentType::PROGRESS_COMPONENT) != temp->getComponents().end()) {
				progressEntity = temp;
				break;
			}
		}

		if (progressEntity != nullptr) {
			ProgressComponentPtr progressBar = progressEntity->getComponentByType<ProgressComponent>(ComponentType::PROGRESS_COMPONENT);
			progressBar->setProgress(progress, maxProgress);
		}
		else {
			int x = 0;
			int y = 0;
			int w = 0;
			int h = 8;
			if (string("top").compare(location) == 0) {}
			if (entity->getComponents().find(ComponentType::DRAWABLE_COMPONENT) != entity->getComponents().end()) {
				DrawableComponentPtr drawableComponent = entity->getComponentByType<DrawableComponent>(ComponentType::DRAWABLE_COMPONENT);
				DrawablePtr drawable = makeShared(drawableComponent->getDrawable());
				w = drawable->width;
				if (string("top").compare(location) == 0) {
					y -= (drawable->height / 2) + 4;
				}
				else if (string("bottom").compare(location) == 0) {
					y += (drawable->height / 2) + 4;
				}
			}
			else if (entity->getComponents().find(ComponentType::ANIMATION_COMPONENT) != entity->getComponents().end()) {
				AnimationComponentPtr drawableComponent = entity->getComponentByType<AnimationComponent>(ComponentType::ANIMATION_COMPONENT);
				w = drawableComponent->aninmationDrawable->width;;
				if (string("top").compare(location) == 0) {
					y -= (drawableComponent->aninmationDrawable->height / 2) + 4;
				}
				else if (string("bottom").compare(location) == 0) {
					y += (drawableComponent->aninmationDrawable->height / 2) + 4;
				}
			}

			progressEntity = mWidgetFactory->createProgressBar(x, y, w, h, maxProgress, progress);
			entity->children.push_back(progressEntity);
			progressEntity->parent = entity;
		}
	};
}

void LuaScriptFactory::registerAsset(LuaScriptPtr script) {
	script->state["loadTexture"] = [this](string path, string tag) {
		GraphicsSystemPtr graphicsSystem = mSystemManager->getSystemByType<GraphicsSystem>(SystemType::GRAPHICS);

		graphicsSystem->addTexture(path, tag);
	};
	script->state["loadFont"] = [this](string path, string tag, int fontsize) {
		GraphicsSystemPtr graphicsSystem = mSystemManager->getSystemByType<GraphicsSystem>(SystemType::GRAPHICS);

		graphicsSystem->addFont(path, tag, fontsize);
	};
}


void LuaScriptFactory::registerCamera(LuaScriptPtr script) {
	script->state["moveCameraBy"] = [this](int dx, int dy) {
		GraphicsSystemPtr graphicsSystem = mSystemManager->getSystemByType<GraphicsSystem>(SystemType::GRAPHICS);

		CameraPtr camera = makeShared(graphicsSystem->getCamera());
		*(camera->position) += Vector2f(dx, dy);
	};
	script->state["setCameraPosition"] = [this](int dx, int dy) {
		GraphicsSystemPtr graphicsSystem = mSystemManager->getSystemByType<GraphicsSystem>(SystemType::GRAPHICS);

		CameraPtr camera = makeShared(graphicsSystem->getCamera());
		camera->position->set(Vector2f(dx, dy));
	};


	script->state["getCameraPosition"] = [this]() -> LuaFriendlyVector2f& {
		GraphicsSystemPtr graphicsSystem = mSystemManager->getSystemByType<GraphicsSystem>(SystemType::GRAPHICS);

		CameraPtr camera = makeShared(graphicsSystem->getCamera());

		auto vec = *(GCC_NEW LuaFriendlyVector2f(const_cast<Vector2f&>(*camera->position)));

		toDelete.push_back(VoidPtr(&vec));

		return vec;
	};
}

void LuaScriptFactory::registerMap(LuaScriptPtr script) {
	script->state["setTileColor"] = [this](int x, int y, int r, int g, int b, int a) {
		MapSystemPtr mapSystem = mSystemManager->getSystemByType<MapSystem>(SystemType::MAP);

		MapPtr map = mapSystem->getMap();
		Vector2f point(x, y);
		CellPtr cell = map->cellAtPoint(point);
	};
}