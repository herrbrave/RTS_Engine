#include"Entity.h"

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

void Entity::pushState(StatePtr state) {
	mStateQueue.push_back(state);
}

void Entity::addComponent(ComponentPtr component) {
	mComponents.emplace(component->componentId, component);
}