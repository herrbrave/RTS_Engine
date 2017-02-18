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

	EntityPtr createDefault(float x, float y, float width, float height, Uint8 r, Uint8 g, Uint8 b, Uint8 a);

	EntityPtr createTexturedEntity(const string& assetTag, float x, float y, float width, float height, float tx, float ty, float w, float h, bool isCollidable);

	EntityPtr createAnimatedEntity(const string& path, float width, float height);

	EntityPtr createFromSerialization(const string& path);

	EntityPtr createPhysicsEntity(float x, float y, float width, float height);

protected:
	SystemManagerPtr mSystemManager{ nullptr };
};

#endif // !__ENTITY_FACTORY_H__
