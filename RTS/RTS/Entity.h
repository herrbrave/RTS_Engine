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
#include"Constants.h"
#include"State.h"
#include"Vector2f.h"

static std::atomic_ulong sEntityId{ 1 };
static std::atomic_ulong sEntityFactoryId{ 1 };

class Entity;
typedef shared_ptr<Entity> EntityPtr;
typedef weak_ptr<Entity> WeakEntityPtr;

class EntityVendor;
typedef shared_ptr<EntityVendor> EntityVendorPtr;
typedef weak_ptr<EntityVendor> WeakEntityVendorPtr;

typedef unordered_map<ComponentType, shared_ptr<Component>> Components;

class Entity {
public:
	unsigned long parent = -1;
	std::vector<unsigned long> children;

	Entity(){}

	const unsigned long id = sEntityId++;
	Uint8 type;

	virtual void update();
	void pushState(StatePtr state);

	template<class ClassType>
	weak_ptr<ClassType> getComponentByType(ComponentType componentType) const {
		auto it = mComponents.find(componentType);
		if (it == mComponents.end()) {
			return weak_ptr<ClassType>();
		}

		shared_ptr<Component> component = it->second;
		shared_ptr<ClassType> converted = static_pointer_cast<ClassType>(component);
		return weak_ptr<ClassType>(converted);
	}

	void addComponent(ComponentPtr component); 

	void serialize(Serializer& serializer) const {
		serializer.writer.StartObject();

		serializer.writer.String("components");
		serializer.writer.StartArray();
		for (auto entry : mComponents) {
			entry.second->serialize(serializer);
		}
		serializer.writer.EndArray();

		serializer.writer.EndObject();
	}

protected:
	Components mComponents;
	vector<std::shared_ptr<State>> mStateQueue;
};

class EntityVendor {
public:
	virtual WeakEntityPtr getEntityById(unsigned long entityId) = 0;
};

#endif // !__ENTITY_H__
