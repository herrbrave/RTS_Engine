#ifndef __COMPONENT_H__
#define __COMPONENT_H__

#include"Constants.h"
#include"Serializer.h"

#include<algorithm>
#include<atomic>
#include<memory>
#include<SDL.h>
#include<unordered_map>

enum class ComponentType : Uint8 {
	DRAWABLE_COMPONENT = 1,
	PHYSICS_COMPONENT = 2,
	TILE_COMPONENT = 3,
	BUTTON_COMPONENT = 4,
	LABEL_COMPONENT = 5,
	INPUT_COMPONENT = 6,
	ANIMATION_COMPONENT = 7,
	LUA_SCRIPT_COMPONENT = 8,
	PROGRESS_COMPONENT = 9,
	PARTICLE_COMPONENT = 10,
	GRID_COMPONENT = 11,
	ITEM_PANEL_COMPONENT = 12,
	TEXT_BOX_COMPONENT = 13,
};

class Component;
typedef shared_ptr<Component> ComponentPtr;
typedef weak_ptr<Component> WeakComponentPtr;

class Component {
public:
	unsigned int entityId;
	ComponentType componentId;

	Component(unsigned long entityId, ComponentType componentId) {
		this->entityId = entityId;
		this->componentId = componentId;
	}

	virtual void serialize(Serializer& serializer) const = 0;
};

#endif // !__COMPONENT_H__
