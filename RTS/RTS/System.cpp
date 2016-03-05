#include"System.h"

SystemManager::SystemManager(GraphicsConfig* graphicsConfig) {
	systems.emplace(SystemType::ASSET, new AssetSystem(this));
	systems.emplace(SystemType::ENTITY, new EntitySystem(this));
	systems.emplace(SystemType::PHYSICS, new PhysicsSystem(this));
	systems.emplace(SystemType::GRAPHICS, new GraphicsSystem(graphicsConfig, this));
}


Asset* AssetSystem::DefaultAssetVendor::getAsset(const std::string& tag) {
	return mAssetSystem->getAsset(tag);
}

void AssetSystem::clear() {
	mAssets.clear();
}

void GraphicsSystem::registerDrawable(const unsigned long id, Drawable* drawable) {
	mDrawables.emplace(id, drawable);
}

void  GraphicsSystem::deregisterDrawable(const unsigned long id) {
	mDrawables.erase(mDrawables.find(id));
}

void  GraphicsSystem::draw() {
	mGraphics->onBeforeDraw();

	for (auto drawable : mDrawables) {
		Entity* entity = getEntityById(drawable.first, mSystemManager);
		vector2f position;
		getEntityPosition(&position, entity, mSystemManager);
		if (!drawable.second->isUi) {
			translateToCamera(&position, mCamera.get());
		}

		drawable.second->draw(mGraphics.get(), &position);
	}

	mGraphics->onAfterDraw();
}

Camera* GraphicsSystem::getCamera() {
	return mCamera.get();
}

Drawable* GraphicsSystem::getDrawableById(unsigned long entityId) {
	return mDrawables.at(entityId);
}

void GraphicsSystem::addTexture(const std::string& path, const std::string& assetTag) {
	if (assetExists(assetTag, mSystemManager)) {
		SDL_Log("Asset already loaded.");
		return;
	}
	AssetSystem* assetSystem = reinterpret_cast<AssetSystem*>(mSystemManager->systems.at(SystemType::ASSET));

	assetSystem->registerAsset(mGraphics->createTexture(path, assetTag));
}

void GraphicsSystem::createTextSurface(const std::string& text, const std::string& assetTag, Uint8 r, Uint8 g, Uint8 b, Uint8 a) {
	if (assetExists(assetTag, mSystemManager)) {
		SDL_Log("Text already added.");
		return;
	}
	AssetSystem* assetSystem = reinterpret_cast<AssetSystem*>(mSystemManager->systems.at(SystemType::ASSET));

	assetSystem->registerAsset(mGraphics->createTextAsset(text, assetTag, r, g, b, a));
}

void GraphicsSystem::clear() {
	mDrawables.clear();
}

void EntitySystem::registerEntity(Entity* entity) {
	mEntityMap.emplace(entity->id, entity);
}

Entity* EntitySystem::getEntityById(unsigned long id) {
	return mEntityMap.at(id);
}

Entity* EntitySystem::DefaultEntityVendor::getEntityById(unsigned long entityId) {
	return mEntitySystem->getEntityById(entityId);
}

void EntitySystem::deregisterEntity(unsigned long id) {
	mEntityMap.erase(mEntityMap.find(id));
}

void EntitySystem::clear() {
	mEntityMap.clear();
}

void PhysicsSystem::registerBody(const unsigned long id, Body* body) {
	mBodies.emplace(id, std::shared_ptr<Body>(body));
}

void PhysicsSystem::deregisterBody(const unsigned long id) {
	mBodies.erase(mBodies.find(id));
}


Body* PhysicsSystem::getBody(const unsigned long id) {
	return mBodies.at(id).get();
}

void PhysicsSystem::update(Uint32 delta) {
	float step(float(delta) / float(1000));


	// TODO: add steering later.
	for (auto element : mBodies) {
		vector2f velocityCopy(*element.second->getVelocity());
		if (velocityCopy.x == 0 && velocityCopy.y == 0) {
			continue;
		}

		velocityCopy *= element.second->getSpeed();
		velocityCopy *= step;

		vector2f positionCopy(*element.second->getPosition());
		positionCopy += velocityCopy;

		element.second->setPosition(&positionCopy);
	}
}

void PhysicsSystem::clear() {
	mBodies.clear();
}

void getEntityPosition(vector2f* vector, Entity* entity, SystemManager* systemManager) {
	PhysicsComponent* physicsComponent = reinterpret_cast<PhysicsComponent*>(entity->componentContainer->getComponentByType(ComponentType::PHYSICS_COMPONENT));
	vector->set(physicsComponent->getPosition()->x, physicsComponent->getPosition()->y);

	EntitySystem* entitySystem = reinterpret_cast<EntitySystem*>(systemManager->systems.at(SystemType::ENTITY));

	unsigned long parentId = entity->parent;
	while (parentId != -1) {
		Entity* parent = entitySystem->getEntityById(parentId);
		physicsComponent = reinterpret_cast<PhysicsComponent*>(parent->componentContainer->getComponentByType(ComponentType::PHYSICS_COMPONENT));
		*vector += *physicsComponent->getPosition();
		parentId = parent->parent;
	}
}

Entity* getEntityById(unsigned long entityId, SystemManager* systemManager) {
	EntitySystem* entitySystem = reinterpret_cast<EntitySystem*>(systemManager->systems.at(SystemType::ENTITY));
	return entitySystem->getEntityById(entityId);
}

bool assetExists(const std::string& assetTag, SystemManager* systemManager) {
	AssetSystem* assetSystem = reinterpret_cast<AssetSystem*>(systemManager->systems.at(SystemType::ASSET));
	return assetSystem->contains(assetTag);
}

Asset* getAsset(const std::string& assetTag, SystemManager* systemManager) {
	AssetSystem* assetSystem = reinterpret_cast<AssetSystem*>(systemManager->systems.at(SystemType::ASSET));
	return assetSystem->getAsset(assetTag);
}

void updatePhysicsSystem(Uint32 ticks, SystemManager* systemManager) {
	PhysicsSystem* physicsSystem = reinterpret_cast<PhysicsSystem*>(systemManager->systems.at(SystemType::PHYSICS));
	physicsSystem->update(ticks);
}

void drawGraphicsSystem(SystemManager* systemManager) {
	GraphicsSystem* graphicsSystem = reinterpret_cast<GraphicsSystem*>(systemManager->systems.at(SystemType::GRAPHICS));
	graphicsSystem->draw();
}

Drawable* getDrawableById(unsigned long drawableId, SystemManager* systemManager) {
	GraphicsSystem* graphicsSystem = reinterpret_cast<GraphicsSystem*>(systemManager->systems.at(SystemType::GRAPHICS));
	return graphicsSystem->getDrawableById(drawableId);
}

Body* getBodyById(unsigned long bodyId, SystemManager* systemManager) {
	PhysicsSystem* physicsSystem = reinterpret_cast<PhysicsSystem*>(systemManager->systems.at(SystemType::PHYSICS));
	return physicsSystem->getBody(bodyId);
}