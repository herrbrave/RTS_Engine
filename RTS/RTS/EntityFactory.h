#ifndef __ENTITY_FACTORY_H__
#define __ENTITY_FACTORY_H__

#include"Entity.h"
#include"Map.h"
#include"Serializer.h"
#include"System.h"

class EntityFactory;
typedef shared_ptr<EntityFactory> EntityFactoryPtr;
typedef weak_ptr<EntityFactory> WeakEntityFactoryPtr;

class EntityFactory {
public:
	EntityFactory(SystemManagerPtr systemManager);

	EntityPtr createDefault();

	EntityPtr createTexturedEntity(const string& assetTag, float tx, float ty, float w, float h);

	EntityPtr createAnimatedEntity(const string& path, float width, float height);

	EntityPtr createFromSerialization(const string& path);

	EntityPtr createPhysicsEntity(float x, float y, float width, float height);

protected:
	SystemManagerPtr mSystemManager{ nullptr };
};

#endif // !__ENTITY_FACTORY_H__
