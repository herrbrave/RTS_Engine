#ifndef __ENTITY_FACTORY_H__
#define __ENTITY_FACTORY_H__

#include"Entity.h"
#include"Map.h"
#include"Serializer.h"
#include"System.h"


class EntityFactory {
public:
	EntityFactory(SystemManager* systemManager);

	Entity* createDefault();

	Entity* createTexturedEntity(std::string assetTag, float tx, float ty, float w, float h);

	Entity* createFromSerialization(std::string path);

	Entity* createPhysicsEntity(float x, float y, float width, float height);

protected:
	SystemManager* mSystemManager{ nullptr };
};

#endif // !__ENTITY_FACTORY_H__
