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
typedef vector<vector<EntityPtr>> EntityMatrix;

class EntityVendor;
typedef shared_ptr<EntityVendor> EntityVendorPtr;
typedef weak_ptr<EntityVendor> WeakEntityVendorPtr;

typedef unordered_map<ComponentType, shared_ptr<Component>> Components;

class Entity : public Serializable {
public:
public:
	const unsigned long id = sEntityId++;
	EntityPtr parent{ nullptr };
	std::vector<EntityPtr> children;

	Entity() {}

	Entity(const rapidjson::Value& root);

	virtual void update();

	void pushState(StatePtr state);

	template<class ClassType>
	shared_ptr<ClassType> getComponentByType(ComponentType componentType) const {
		auto it = mComponents.find(componentType);
		if (it == mComponents.end()) {
			return shared_ptr<ClassType>();
		}

		shared_ptr<Component> component = it->second;
		shared_ptr<ClassType> converted = static_pointer_cast<ClassType>(component);
		return converted;
	}


	Components& getComponents();

	void addComponent(ComponentPtr component); 

	void serialize(Serializer& serializer) const override;

protected:
	Components mComponents;
	vector<std::shared_ptr<State>> mStateQueue;
};

class EntityVendor {
public:
	virtual EntityPtr getEntityById(unsigned long entityId) = 0;
};

#endif // !__ENTITY_H__
