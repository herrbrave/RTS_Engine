#include"Entity.h"

Entity::Entity(Body* body, Drawable* drawable) {
	mBody = body;
	mDrawable = drawable;
}

Body* Entity::getBody() {
	return mBody;
}

Drawable* Entity::getDrawable() {
	return mDrawable;
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

EntityFactory::EntityFactory(GraphicsSystem* graphics, PhysicsSystem* physics) {
	mGraphicsSystem = graphics;
	mPhysicsSystem = physics;
}
