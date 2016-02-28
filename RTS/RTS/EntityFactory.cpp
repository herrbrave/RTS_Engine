#include"EntityFactory.h"

EntityFactory::EntityFactory(SystemManager* systemManager) {
	mSystemManager = systemManager;
}

Entity* EntityFactory::createDefault() {
	// TODO: Make the create method take a json blob config so I can create from serialization.
	Entity* entity = new Entity();
	mSystemManager->entitySystem->registerEntity(entity);

	Body* blockBody = new BlockBody(0, 0, 32, 32);
	mSystemManager->physicsSystem->registerBody(entity->id, blockBody);
	PhysicsComponent* physicsComponent = new PhysicsComponent(entity->id, blockBody);

	Drawable* blockDrawable = new BlockDrawable(32, 32, 255, 0, 0, 255);
	mSystemManager->graphicsSystem->registerDrawable(entity->id, blockDrawable);
	DrawableComponent* drawableComponent = new DrawableComponent(entity->id, blockDrawable);

	entity->componentContainer->registerComponent(physicsComponent);
	entity->componentContainer->registerComponent(drawableComponent);

	return entity;
}

Entity* EntityFactory::createTexturedEntity(std::string assetTag, float tx, float ty, float w, float h) {
	// TODO: Make the create method take a json blob config so I can create from serialization.
	Entity* entity = new Entity();
	mSystemManager->entitySystem->registerEntity(entity);

	Body* blockBody = new BlockBody(0, 0, w, h);
	mSystemManager->physicsSystem->registerBody(entity->id, blockBody);
	PhysicsComponent* physicsComponent = new PhysicsComponent(entity->id, blockBody);

	Texture* texture = new Texture(assetTag, tx, ty, w, h);
	Drawable* textureDrawable = new TextureDrawable(texture);
	mSystemManager->graphicsSystem->registerDrawable(entity->id, textureDrawable);
	DrawableComponent* drawableComponent = new DrawableComponent(entity->id, textureDrawable);

	entity->componentContainer->registerComponent(physicsComponent);
	entity->componentContainer->registerComponent(drawableComponent);

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

	Entity* entity = new Entity();
	mSystemManager->entitySystem->registerEntity(entity);
	const rapidjson::Value& componentContainer = doc["componentContainer"];

	for (rapidjson::SizeType index = 0; index < componentContainer.Size(); index++) {
		const rapidjson::Value& component = componentContainer[index];
		Uint8 componentId = component["componentId"].GetUint();

		if (componentId == ComponentType::DRAWABLE_COMPONENT) {
			DrawableComponent* comp = new DrawableComponent(entity->id, component);
			entity->componentContainer->registerComponent(comp);
			mSystemManager->graphicsSystem->registerDrawable(entity->id, comp->getDrawable());
		}
		else if (componentId == ComponentType::PHYSICS_COMPONENT) {
			PhysicsComponent* comp = new PhysicsComponent(entity->id, component);
			entity->componentContainer->registerComponent(comp);
			mSystemManager->physicsSystem->registerBody(entity->id, comp->getBody());
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