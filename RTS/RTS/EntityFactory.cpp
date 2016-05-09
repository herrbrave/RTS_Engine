#include"EntityFactory.h"

EntityFactory::EntityFactory(SystemManagerPtr systemManager) {
	mSystemManager = systemManager;
}

EntityPtr EntityFactory::createDefault() {
	EntitySystemPtr entitySystem = makeShared(mSystemManager->getSystemByType<EntitySystem>(SystemType::ENTITY));
	EntityPtr entity(GCC_NEW Entity());
	entitySystem->addEntity(entity);

	PhysicsSystemPtr physicsSystem = makeShared(mSystemManager->getSystemByType<PhysicsSystem>(SystemType::PHYSICS));
	BodyPtr blockBody(GCC_NEW Body(0, 0, 16, 16));
	physicsSystem->registerBody(entity->id, blockBody);
	PhysicsComponentPtr physicsComponent(GCC_NEW PhysicsComponent(entity->id, blockBody, physicsSystem->physicsNotifier));
	ColliderPtr collider(GCC_NEW Collider(0, 0, 16, 16));
	physicsComponent->setCollider(collider);

	GraphicsSystemPtr graphicsSystem = makeShared(mSystemManager->getSystemByType<GraphicsSystem>(SystemType::GRAPHICS));
	DrawablePtr blockDrawable(GCC_NEW BlockDrawable(16, 16, 255, 0, 0, 255));
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

EntityPtr EntityFactory::createTexturedEntity(const string& assetTag, float tx, float ty, float w, float h) {
	EntitySystemPtr entitySystem = makeShared(mSystemManager->getSystemByType<EntitySystem>(SystemType::ENTITY));
	EntityPtr entity(GCC_NEW Entity());
	entitySystem->addEntity(entity);

	PhysicsSystemPtr physicsSystem = makeShared(mSystemManager->getSystemByType<PhysicsSystem>(SystemType::PHYSICS));
	BodyPtr blockBody(GCC_NEW Body(0.0f, 0.0f, w, h));
	physicsSystem->registerBody(entity->id, blockBody);
	PhysicsComponentPtr physicsComponent(GCC_NEW PhysicsComponent(entity->id, blockBody, physicsSystem->physicsNotifier));

	GraphicsSystemPtr graphicsSystem = makeShared(mSystemManager->getSystemByType<GraphicsSystem>(SystemType::GRAPHICS));
	TexturePtr texture(GCC_NEW Texture(assetTag, tx, ty, w, h));
	DrawablePtr textureDrawable(GCC_NEW TextureDrawable(texture));
	graphicsSystem->registerDrawable(entity->id, textureDrawable);
	DrawableComponentPtr drawableComponent(GCC_NEW DrawableComponent(entity->id, textureDrawable));

	InputSystemPtr inputSystem = makeShared(mSystemManager->getSystemByType<InputSystem>(SystemType::INPUT));
	InputListenerPtr inputListener(GCC_NEW InputListener(entity->id));
	inputSystem->registerEventListener(inputListener);
	InputComponentPtr inputComponent(GCC_NEW InputComponent(entity->id, inputListener));

	entity->addComponent(ComponentPtr(physicsComponent));
	entity->addComponent(ComponentPtr(drawableComponent));
	entity->addComponent(ComponentPtr(inputComponent));

	return entity;
}

EntityPtr EntityFactory::createAnimatedEntity(const string& path, float width, float height) {
	EntitySystemPtr entitySystem = makeShared(mSystemManager->getSystemByType<EntitySystem>(SystemType::ENTITY));
	EntityPtr entity(GCC_NEW Entity());
	entitySystem->addEntity(entity);

	PhysicsSystemPtr physicsSystem = makeShared(mSystemManager->getSystemByType<PhysicsSystem>(SystemType::PHYSICS));
	BodyPtr blockBody(GCC_NEW Body(0.0f, 0.0f, width, height));
	physicsSystem->registerBody(entity->id, blockBody);
	PhysicsComponentPtr physicsComponent(GCC_NEW PhysicsComponent(entity->id, blockBody, physicsSystem->physicsNotifier));

	GraphicsSystemPtr graphicsSystem = makeShared(mSystemManager->getSystemByType<GraphicsSystem>(SystemType::GRAPHICS));
	TexturePtr texture(GCC_NEW Texture(""));
	shared_ptr<TextureDrawable> textureDrawable(GCC_NEW TextureDrawable(texture));
	graphicsSystem->registerDrawable(entity->id, textureDrawable);
	DrawableComponentPtr drawableComponent(GCC_NEW DrawableComponent(entity->id, textureDrawable));

	AnimationSystemPtr animationSystem = makeShared(mSystemManager->getSystemByType<AnimationSystem>(SystemType::ANIMATION));
	AnimationSetPtr animationSet = animationSystem->createAnimationSet(path);
	AnimationHandlerPtr animationHandler(GCC_NEW AnimationHandler(textureDrawable, animationSet, animationSet->fps));
	animationSystem->registerAnimation(entity->id, animationHandler);
	AnimationComponentPtr animationComponent(GCC_NEW AnimationComponent(entity->id, animationHandler));

	InputSystemPtr inputSystem(makeShared(mSystemManager->getSystemByType<InputSystem>(SystemType::INPUT)));
	InputListenerPtr inputListener(GCC_NEW InputListener(entity->id));
	inputSystem->registerEventListener(inputListener);
	InputComponentPtr inputComponent(GCC_NEW InputComponent(entity->id, inputListener));

	entity->addComponent(ComponentPtr(physicsComponent));
	entity->addComponent(ComponentPtr(drawableComponent));
	entity->addComponent(ComponentPtr(animationComponent));
	entity->addComponent(ComponentPtr(inputComponent));

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
	const rapidjson::Value& componentContainer = doc["componentContainer"];

	GraphicsSystemPtr graphicsSystem = static_pointer_cast<GraphicsSystem>(mSystemManager->systems.at(SystemType::GRAPHICS));
	PhysicsSystemPtr physicsSystem = static_pointer_cast<PhysicsSystem>(mSystemManager->systems.at(SystemType::PHYSICS));
	for (rapidjson::SizeType index = 0; index < componentContainer.Size(); index++) {
		const rapidjson::Value& component = componentContainer[index];
		ComponentType componentId = (ComponentType) component["componentId"].GetUint();

		if (componentId == ComponentType::DRAWABLE_COMPONENT) {
			DrawableComponentPtr comp(GCC_NEW DrawableComponent(entity->id, component));
			entity->addComponent(ComponentPtr(comp));
			graphicsSystem->registerDrawable(entity->id, makeShared(comp->getDrawable()));
		}
		else if (componentId == ComponentType::PHYSICS_COMPONENT) {
			PhysicsComponentPtr comp(GCC_NEW PhysicsComponent(entity->id, component, physicsSystem->physicsNotifier));
			entity->addComponent(ComponentPtr(comp));
			physicsSystem->registerBody(entity->id, makeShared(comp->getBody()));
		}
		else if (componentId == ComponentType::TILE_COMPONENT) {
			entity->addComponent(ComponentPtr(GCC_NEW TileComponent(entity->id, component)));
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
	BodyPtr blockBody(GCC_NEW Body(x, y, width, height));
	physicsSystem->registerBody(entity->id, blockBody);
	PhysicsComponentPtr physicsComponent(GCC_NEW PhysicsComponent(entity->id, blockBody, physicsSystem->physicsNotifier));
	ColliderPtr collider(GCC_NEW Collider(x, y, width, height));
	physicsComponent->setCollider(collider);

	entity->addComponent(ComponentPtr(physicsComponent));

	return entity;
}