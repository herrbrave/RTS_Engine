#include"EntityFactory.h"

EntityFactory::EntityFactory(SystemManager* systemManager) {
	mSystemManager = systemManager;
}

Entity* EntityFactory::createDefault() {
	// TODO: Make the create method take a json blob config so I can create from serialization.
	EntitySystem* entitySystem = reinterpret_cast<EntitySystem*>(mSystemManager->systems.at(SystemType::ENTITY));
	Entity* entity = new Entity();
	entitySystem->registerEntity(entity);

	PhysicsSystem* physicsSystem = reinterpret_cast<PhysicsSystem*>(mSystemManager->systems.at(SystemType::PHYSICS));
	Body* blockBody = new Body(0, 0, 16, 16);
	physicsSystem->registerBody(entity->id, blockBody);
	PhysicsComponent* physicsComponent = new PhysicsComponent(entity->id, blockBody, physicsSystem->physicsNotifier.get());
	Collider* collider = new Collider(0, 0, 16, 16);
	physicsComponent->setCollider(collider);

	GraphicsSystem* graphicsSystem = reinterpret_cast<GraphicsSystem*>(mSystemManager->systems.at(SystemType::GRAPHICS));
	Drawable* blockDrawable = new BlockDrawable(16, 16, 255, 0, 0, 255);
	graphicsSystem->registerDrawable(entity->id, blockDrawable);
	DrawableComponent* drawableComponent = new DrawableComponent(entity->id, blockDrawable);

	InputSystem* inputSystem = reinterpret_cast<InputSystem*>(mSystemManager->systems.at(SystemType::INPUT));
	InputListener* inputListener = new InputListener(entity->id);
	inputSystem->registerEventListener(inputListener);
	InputComponent* inputComponent = new InputComponent(entity->id, inputListener);

	entity->componentContainer->registerComponent(physicsComponent);
	entity->componentContainer->registerComponent(drawableComponent);
	entity->componentContainer->registerComponent(inputComponent);

	return entity;
}

Entity* EntityFactory::createTexturedEntity(std::string assetTag, float tx, float ty, float w, float h) {
	// TODO: Make the create method take a json blob config so I can create from serialization.
	EntitySystem* entitySystem = reinterpret_cast<EntitySystem*>(mSystemManager->systems.at(SystemType::ENTITY));
	Entity* entity = new Entity();
	entitySystem->registerEntity(entity);

	PhysicsSystem* physicsSystem = reinterpret_cast<PhysicsSystem*>(mSystemManager->systems.at(SystemType::PHYSICS));
	Body* blockBody = new Body(0, 0, w, h);
	physicsSystem->registerBody(entity->id, blockBody);
	PhysicsComponent* physicsComponent = new PhysicsComponent(entity->id, blockBody, physicsSystem->physicsNotifier.get());

	GraphicsSystem* graphicsSystem = reinterpret_cast<GraphicsSystem*>(mSystemManager->systems.at(SystemType::GRAPHICS));
	Texture* texture = new Texture(assetTag, tx, ty, w, h);
	Drawable* textureDrawable = new TextureDrawable(texture);
	graphicsSystem->registerDrawable(entity->id, textureDrawable);
	DrawableComponent* drawableComponent = new DrawableComponent(entity->id, textureDrawable);

	InputSystem* inputSystem = reinterpret_cast<InputSystem*>(mSystemManager->systems.at(SystemType::INPUT));
	InputListener* inputListener = new InputListener(entity->id);
	inputSystem->registerEventListener(inputListener);
	InputComponent* inputComponent = new InputComponent(entity->id, inputListener);

	entity->componentContainer->registerComponent(physicsComponent);
	entity->componentContainer->registerComponent(drawableComponent);
	entity->componentContainer->registerComponent(inputComponent);

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

	EntitySystem* entitySystem = reinterpret_cast<EntitySystem*>(mSystemManager->systems.at(SystemType::ENTITY));
	Entity* entity = new Entity();
	entitySystem->registerEntity(entity);
	const rapidjson::Value& componentContainer = doc["componentContainer"];

	GraphicsSystem* graphicsSystem = reinterpret_cast<GraphicsSystem*>(mSystemManager->systems.at(SystemType::GRAPHICS));
	PhysicsSystem* physicsSystem = reinterpret_cast<PhysicsSystem*>(mSystemManager->systems.at(SystemType::PHYSICS));
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