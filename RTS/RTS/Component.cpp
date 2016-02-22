#include"Component.h"


void ComponentContainer::registerComponent(Component* component) {
	unsigned long id = component->componentId;
	if (mComponents.find(id) != mComponents.end()) {
		throw std::exception("Component type already registered.");
	}

	mComponents.emplace(id, std::shared_ptr<Component>(component));
}

void ComponentContainer::deregisterComponent(unsigned long id) {
	if (mComponents.find(id) == mComponents.end()) {
		throw std::exception("No component registered.");
	}

	mComponents.erase(id);
}

Component* ComponentContainer::getComponentByType(unsigned long type) {
	return mComponents.at(type).get();
}