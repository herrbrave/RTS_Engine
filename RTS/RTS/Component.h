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
static const unordered_map<Uint8, ComponentType> COMPONENT_VALUE_MAP = {
	{(Uint8)1, ComponentType::DRAWABLE_COMPONENT},
	{(Uint8)2, ComponentType::PHYSICS_COMPONENT},
	{(Uint8)4, ComponentType::BUTTON_COMPONENT},
	{(Uint8)5, ComponentType::LABEL_COMPONENT},
	{(Uint8)6, ComponentType::INPUT_COMPONENT},
	{(Uint8)7, ComponentType::ANIMATION_COMPONENT},
	{(Uint8)8, ComponentType::LUA_SCRIPT_COMPONENT},
	{(Uint8)9, ComponentType::PROGRESS_COMPONENT},
	{(Uint8)10, ComponentType::PARTICLE_COMPONENT},
	{(Uint8)11, ComponentType::GRID_COMPONENT},
	{(Uint8)12, ComponentType::ITEM_PANEL_COMPONENT},
	{(Uint8)13, ComponentType::TEXT_BOX_COMPONENT}
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
	
	virtual ~Component() {}

	virtual void serialize(Serializer& serializer) const = 0;
};

#endif // !__COMPONENT_H__
