#ifndef __ENTITY_H__
#define __ENTITY_H__

#include<atomic>
#include<vector>

#include"Component.h"
#include"Graphics.h"
#include "Physics.h"
#include"State.h"
#include"vector2f.h"

static std::atomic_ulong sEntityId{ 1 };
static std::atomic_ulong sEntityFactoryId{ 1 };

class Entity {
public:
	Entity();

	const unsigned long id = sEntityId.fetch_add(1);
	Uint8 type;
	std::unique_ptr<ComponentContainer> componentContainer;

	virtual void update();
	void pushState(std::shared_ptr<State> state);
protected:
	std::vector<std::shared_ptr<State>> mStateQueue;
};

class EntityFactory {
public:
	EntityFactory(GraphicsSystem* graphics, PhysicsSystem* physics);

	Entity* create();

protected:
	GraphicsSystem* mGraphicsSystem{ nullptr };
	PhysicsSystem* mPhysicsSystem{ nullptr };
};

#endif // !__ENTITY_H__
