#include"Entity.h"

Entity::Entity() {
	componentContainer.reset(new ComponentContainer());
}

void Entity::update() {
	if (mStateQueue.size() <= 0) {
		return;
	}

	auto currentState = mStateQueue[0];
	if (!currentState->hasBegun()) {
		currentState->begin();
	}

	if (currentState->isEnded()) {
		mStateQueue.erase(std::begin(mStateQueue));
		if (mStateQueue.size() > 0) {
			currentState = mStateQueue[0];
			currentState->begin();
		}
		else {
			currentState = nullptr;
		}
	}

	if (currentState != nullptr) {
		currentState->update();
	}
}

void Entity::pushState(std::shared_ptr<State> state) {
	mStateQueue.push_back(state);
}

EntityFactory::EntityFactory(EntitySystem* entity, GraphicsSystem* graphics, PhysicsSystem* physics) {
	mEntitySystem = entity;
	mGraphicsSystem = graphics;
	mPhysicsSystem = physics;
}

Entity* EntityFactory::createDefault() {
	// TODO: Make the create method take a json blob config so I can create from serialization.
	Entity* entity = new Entity();
	mEntitySystem->registerEntity(entity);
	
	Body* blockBody = new BlockBody(0, 0, 32, 32);
	mPhysicsSystem->registerBody(entity->id, blockBody);
	PhysicsComponent* physicsComponent = new PhysicsComponent(entity->id, blockBody);

	Drawable* blockDrawable = new BlockDrawable(32, 32, 255, 0, 0, 255);
	mGraphicsSystem->registerDrawable(entity->id, blockDrawable);
	DrawableComponent* drawableComponent = new DrawableComponent(entity->id, blockDrawable);

	entity->componentContainer->registerComponent(physicsComponent);
	entity->componentContainer->registerComponent(drawableComponent);

	return entity;
}

Entity* EntityFactory::createTexturedEntity(std::string assetTag, float tx, float ty, float w, float h) {
	// TODO: Make the create method take a json blob config so I can create from serialization.
	Entity* entity = new Entity();
	mEntitySystem->registerEntity(entity);

	Body* blockBody = new BlockBody(0, 0, w, h);
	mPhysicsSystem->registerBody(entity->id, blockBody);
	PhysicsComponent* physicsComponent = new PhysicsComponent(entity->id, blockBody);

	Texture* texture = new Texture(assetTag, tx, ty, w, h);
	Drawable* textureDrawable = new TextureDrawable(texture);
	mGraphicsSystem->registerDrawable(entity->id, textureDrawable);
	DrawableComponent* drawableComponent = new DrawableComponent(entity->id, textureDrawable);

	entity->componentContainer->registerComponent(physicsComponent);
	entity->componentContainer->registerComponent(drawableComponent);

	return entity;
}

void EntitySystem::registerEntity(Entity* entity) {
	mEntityMap.emplace(entity->id, entity);
}

Entity* EntitySystem::getEntityById(unsigned long id) {
	return mEntityMap.at(id);
}

void EntitySystem::deregisterEntity(unsigned long id) {
	mEntityMap.erase(mEntityMap.find(id));
}