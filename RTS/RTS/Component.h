#ifndef __COMPONENT_H__
#define __COMPONENT_H__

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
};

class ComponentContainer {
public:
	void registerComponent(Component* component);
	void deregisterComponent(unsigned long id);

	Component* getComponentByType(unsigned long type);

private:
	std::unordered_map<unsigned long, std::shared_ptr<Component>> mComponents;
};

#endif // !__COMPONENT_H__
