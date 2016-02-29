#ifndef __COMPONENT_H__
#define __COMPONENT_H__

#include"Serializer.h"

#include<algorithm>
#include<atomic>
#include<memory>
#include<SDL.h>
#include<unordered_map>

struct ComponentType {
	static const Uint8 DRAWABLE_COMPONENT = 1;
	static const Uint8 PHYSICS_COMPONENT = 2;
	static const Uint8 TILE_COMPONENT = 3;
	static const Uint8 BUTTON_COMPONENT = 4;
};

class Component {
public:
	unsigned int entityId;
	Uint8 componentId;

	Component(unsigned long entityId, Uint8 componentId) {
		this->entityId = entityId;
		this->componentId = componentId;
	}

	virtual void serialize(Serializer& serializer) const = 0;
};

class ComponentContainer {
public:
	ComponentContainer() {}

	void registerComponent(Component* component);
	void deregisterComponent(Uint8 id);

	Component* getComponentByType(Uint8 type);

	void serialize(Serializer& serializer) const {
		serializer.writer.StartArray();

		for (auto component : mComponents) {
			component.second->serialize(serializer);
		}

		serializer.writer.EndArray();
	}

private:
	std::unordered_map<Uint8, Component*> mComponents;
};

#endif // !__COMPONENT_H__
