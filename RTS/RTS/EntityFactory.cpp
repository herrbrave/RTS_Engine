#include"EntityFactory.h"

EntityFactory::EntityFactory(SystemManager* systemManager) {
	mSystemManager = systemManager;
}

Entity* EntityFactory::createDefault() {
	EntitySystemPtr entitySystem = static_pointer_cast<EntitySystem>(mSystemManager->systems.at(SystemType::ENTITY));
	Entity* entity = new Entity();
	entitySystem->registerEntity(entity);

	PhysicsSystemPtr physicsSystem = static_pointer_cast<PhysicsSystem>(mSystemManager->systems.at(SystemType::PHYSICS));
	Body* blockBody = new Body(0, 0, 16, 16);
	physicsSystem->registerBody(entity->id, blockBody);
	PhysicsComponent* physicsComponent = new PhysicsComponent(entity->id, blockBody, physicsSystem->physicsNotifier.get());
	Collider* collider = new Collider(0, 0, 16, 16);
	physicsComponent->setCollider(collider);

	GraphicsSystemPtr graphicsSystem = static_pointer_cast<GraphicsSystem>(mSystemManager->systems.at(SystemType::GRAPHICS));
	Drawable* blockDrawable = new BlockDrawable(16, 16, 255, 0, 0, 255);
	graphicsSystem->registerDrawable(entity->id, blockDrawable);
	DrawableComponent* drawableComponent = new DrawableComponent(entity->id, blockDrawable);

	InputSystemPtr inputSystem = static_pointer_cast<InputSystem>(mSystemManager->systems.at(SystemType::INPUT));
	InputListener* inputListener = new InputListener(entity->id);
	inputSystem->registerEventListener(inputListener);
	InputComponent* inputComponent = new InputComponent(entity->id, inputListener);

	entity->componentContainer->registerComponent(physicsComponent);
	entity->componentContainer->registerComponent(drawableComponent);
	entity->componentContainer->registerComponent(inputComponent);

	return entity;
}

Entity* EntityFactory::createTexturedEntity(std::string assetTag, float tx, float ty, float w, float h) {
	EntitySystemPtr entitySystem = static_pointer_cast<EntitySystem>(mSystemManager->systems.at(SystemType::ENTITY));
	Entity* entity = new Entity();
	entitySystem->registerEntity(entity);

	PhysicsSystemPtr physicsSystem = static_pointer_cast<PhysicsSystem>(mSystemManager->systems.at(SystemType::PHYSICS));
	Body* blockBody = new Body(0, 0, w, h);
	physicsSystem->registerBody(entity->id, blockBody);
	PhysicsComponent* physicsComponent = new PhysicsComponent(entity->id, blockBody, physicsSystem->physicsNotifier.get());

	GraphicsSystemPtr graphicsSystem = static_pointer_cast<GraphicsSystem>(mSystemManager->systems.at(SystemType::GRAPHICS));
	Texture* texture = new Texture(assetTag, tx, ty, w, h);
	Drawable* textureDrawable = new TextureDrawable(texture);
	graphicsSystem->registerDrawable(entity->id, textureDrawable);
	DrawableComponent* drawableComponent = new DrawableComponent(entity->id, textureDrawable);

	InputSystemPtr inputSystem = static_pointer_cast<InputSystem>(mSystemManager->systems.at(SystemType::INPUT));
	InputListener* inputListener = new InputListener(entity->id);
	inputSystem->registerEventListener(inputListener);
	InputComponent* inputComponent = new InputComponent(entity->id, inputListener);

	entity->componentContainer->registerComponent(physicsComponent);
	entity->componentContainer->registerComponent(drawableComponent);
	entity->componentContainer->registerComponent(inputComponent);

	return entity;
}

Entity* EntityFactory::createAnimatedEntity(std::string path, float width, float height) {
	EntitySystemPtr entitySystem = static_pointer_cast<EntitySystem>(mSystemManager->systems.at(SystemType::ENTITY));
	Entity* entity = new Entity();
	entitySystem->registerEntity(entity);

	PhysicsSystemPtr physicsSystem = static_pointer_cast<PhysicsSystem>(mSystemManager->systems.at(SystemType::PHYSICS));
	Body* blockBody = new Body(0, 0, width, height);
	physicsSystem->registerBody(entity->id, blockBody);
	PhysicsComponent* physicsComponent = new PhysicsComponent(entity->id, blockBody, physicsSystem->physicsNotifier.get());

	GraphicsSystemPtr graphicsSystem = static_pointer_cast<GraphicsSystem>(mSystemManager->systems.at(SystemType::GRAPHICS));
	TextureDrawable* textureDrawable = new TextureDrawable(new Texture(""));
	graphicsSystem->registerDrawable(entity->id, textureDrawable);
	DrawableComponent* drawableComponent = new DrawableComponent(entity->id, textureDrawable);

	AnimationSystemPtr animationSystem = static_pointer_cast<AnimationSystem>(mSystemManager->systems.at(SystemType::ANIMATION));
	AnimationSet* animationSet = animationSystem->createAnimationSet(path);
	AnimationHandler* animationHandler = new AnimationHandler(textureDrawable, animationSet, animationSet->fps);
	animationSystem->registerAnimation(entity->id, animationHandler);
	AnimationComponent* animationComponent = new AnimationComponent(entity->id, animationHandler);

	InputSystemPtr inputSystem = static_pointer_cast<InputSystem>(mSystemManager->systems.at(SystemType::INPUT));
	InputListener* inputListener = new InputListener(entity->id);
	inputSystem->registerEventListener(inputListener);
	InputComponent* inputComponent = new InputComponent(entity->id, inputListener);

	entity->componentContainer->registerComponent(physicsComponent);
	entity->componentContainer->registerComponent(drawableComponent);
	entity->componentContainer->registerComponent(inputComponent);
	entity->componentContainer->registerComponent(animationComponent);

	return entity;
}

Entity* EntityFactory::createFromSerialization(std::string path) {
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
	Entity* entity = new Entity();
	entitySystem->registerEntity(entity);
	const rapidjson::Value& componentContainer = doc["componentContainer"];

	GraphicsSystemPtr graphicsSystem = static_pointer_cast<GraphicsSystem>(mSystemManager->systems.at(SystemType::GRAPHICS));
	PhysicsSystemPtr physicsSystem = static_pointer_cast<PhysicsSystem>(mSystemManager->systems.at(SystemType::PHYSICS));
	for (rapidjson::SizeType index = 0; index < componentContainer.Size(); index++) {
		const rapidjson::Value& component = componentContainer[index];
		Uint8 componentId = component["componentId"].GetUint();

		if (componentId == ComponentType::DRAWABLE_COMPONENT) {
			DrawableComponent* comp = new DrawableComponent(entity->id, component);
			entity->componentContainer->registerComponent(comp);
			graphicsSystem->registerDrawable(entity->id, comp->getDrawable());
		}
		else if (componentId == ComponentType::PHYSICS_COMPONENT) {
			PhysicsComponent* comp = new PhysicsComponent(entity->id, component, physicsSystem->physicsNotifier.get());
			entity->componentContainer->registerComponent(comp);
			physicsSystem->registerBody(entity->id, comp->getBody());
		}
		else if (componentId == ComponentType::TILE_COMPONENT) {
			entity->componentContainer->registerComponent(new TileComponent(entity->id, component));
		}
		else {
			assert(false);
		}
	}

	return entity;
}

Entity* EntityFactory::createPhysicsEntity(float x, float y, float width, float height) {

	EntitySystemPtr entitySystem = static_pointer_cast<EntitySystem>(mSystemManager->systems.at(SystemType::ENTITY));
	Entity* entity = new Entity();
	entitySystem->registerEntity(entity);

	PhysicsSystemPtr physicsSystem = static_pointer_cast<PhysicsSystem>(mSystemManager->systems.at(SystemType::PHYSICS));
	Body* blockBody = new Body(x, y, width, height);
	physicsSystem->registerBody(entity->id, blockBody);
	PhysicsComponent* physicsComponent = new PhysicsComponent(entity->id, blockBody, physicsSystem->physicsNotifier.get());
	Collider* collider = new Collider(x, y, width, height);
	physicsComponent->setCollider(collider);

	entity->componentContainer->registerComponent(physicsComponent);

	return entity;
}