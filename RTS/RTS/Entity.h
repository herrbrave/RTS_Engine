#ifndef __ENTITY_H__
#define __ENTITY_H__

#include<atomic>
#include<fstream>
#include<iostream>
#include<streambuf>
#include<vector>
#include<rapidjson\document.h>
#include<rapidjson\prettywriter.h>

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

	const unsigned long id = sEntityId++;
	Uint8 type;
	std::unique_ptr<ComponentContainer> componentContainer;

	virtual void update();
	void pushState(std::shared_ptr<State> state);

	void serialize(Serializer& serializer) const {
		serializer.writer.StartObject();

		serializer.writer.String("componentContainer");
		componentContainer->serialize(serializer);

		serializer.writer.EndObject();
	}

protected:
	std::vector<std::shared_ptr<State>> mStateQueue;
};

class EntitySystem {
public:
	void registerEntity(Entity* entity);
	Entity* getEntityById(unsigned long id);
	void deregisterEntity(unsigned long id);

private:
	std::unordered_map<unsigned long, Entity*> mEntityMap;
};

class EntityFactory {
public:
	EntityFactory(EntitySystem* entitySystem, GraphicsSystem* graphics, PhysicsSystem* physics);

	Entity* createDefault();

	Entity* createTexturedEntity(std::string assetTag, float tx, float ty, float w, float h);

	Entity* createFromSerialization(std::string path);

protected:
	EntitySystem* mEntitySystem{ nullptr };
	GraphicsSystem* mGraphicsSystem{ nullptr };
	PhysicsSystem* mPhysicsSystem{ nullptr };
};

#endif // !__ENTITY_H__
