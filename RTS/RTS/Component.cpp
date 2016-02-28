#include"Component.h"


void ComponentContainer::registerComponent(Component* component) {
	Uint8 id = component->componentId;
	if (mComponents.find(id) != mComponents.end()) {
		throw std::exception("Component type already registered.");
	}

	mComponents.emplace(id, component);
}

void ComponentContainer::deregisterComponent(Uint8 id) {
	if (mComponents.find(id) == mComponents.end()) {
		throw std::exception("No component registered.");
	}

	mComponents.erase(id);
}

Component* ComponentContainer::getComponentByType(Uint8 type) {
	return mComponents.at(type);
}