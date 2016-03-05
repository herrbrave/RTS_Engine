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
#include"State.h"
#include"vector2f.h"

static std::atomic_ulong sEntityId{ 1 };
static std::atomic_ulong sEntityFactoryId{ 1 };

class Entity {
public:
	unsigned long parent{ -1 };
	std::vector<unsigned long> children;

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

class EntityVendor {
public:
	virtual Entity* getEntityById(unsigned long entityId) = 0;
};

#endif // !__ENTITY_H__
