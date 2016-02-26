#ifndef __COMPONENT_H__
#define __COMPONENT_H__

#include"Serializer.h"

#include<algorithm>
#include<atomic>
#include<memory>
#include<SDL.h>
#include<unordered_map>

static std::atomic_ulong sComponentId{ 1 };

class Component {
public:
	unsigned int entityId;
	unsigned long componentId;

	Component(unsigned long entityId, unsigned long componentId) {
		this->entityId = entityId;
		this->componentId = componentId;
	}

	virtual void serialize(Serializer& serializer) const = 0;
};

class ComponentContainer {
public:
	void registerComponent(Component* component);
	void deregisterComponent(unsigned long id);

	Component* getComponentByType(unsigned long type);

	void serialize(Serializer& serializer) const {
		serializer.writer.StartArray();

		for (auto component : mComponents) {
			component.second->serialize(serializer);
		}

		serializer.writer.EndArray();
	}

private:
	std::unordered_map<unsigned long, Component*> mComponents;
};

#endif // !__COMPONENT_H__
