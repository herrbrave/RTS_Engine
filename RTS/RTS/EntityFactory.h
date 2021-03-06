#ifndef __ENTITY_FACTORY_H__
#define __ENTITY_FACTORY_H__

#include"Entity.h"
#include"Map.h"
#include"Serializer.h"
#include"System.h"
#include"ScriptComponent.h"

class EntityFactory;
typedef shared_ptr<EntityFactory> EntityFactoryPtr;
typedef weak_ptr<EntityFactory> WeakEntityFactoryPtr;


void applyDrawable(WeakSystemManagerPtr systemManager, unsigned long entityId, float x, float y, float width, float height, Uint8 r, Uint8 g, Uint8 b, Uint8 a);
void applyDrawable(WeakSystemManagerPtr systemManager, unsigned long entityId, const string& texturePath, float tx, float ty, float w, float h);
void applyDrawable(WeakSystemManagerPtr systemManager, unsigned long entityId, TexturePtr texture);
void applyPhysics(WeakSystemManagerPtr systemManager, unsigned long entityId, float x, float y, float w, float h);
void applyPhysics(WeakSystemManagerPtr systemManager, unsigned long entityId, float x, float y, float w, float h, ColliderShapePtr collider);
void applyInput(WeakSystemManagerPtr systemManager, unsigned long entityId, Input input, std::function<bool(EventPtr)>& callback);
void applyAnimation(WeakSystemManagerPtr systemManager, unsigned long entityId, const string& path);
void applyAnimation(WeakSystemManagerPtr systemManager, unsigned long entityId, AnimationSetPtr animationSet);
void applyScript(WeakSystemManagerPtr systemManager, unsigned long entityId, const string& script);


class EntityFactory {
public:
	EntityFactory(SystemManagerPtr systemManager);

	EntityPtr createDefault(float x, float y, float width, float height, Uint8 r, Uint8 g, Uint8 b, Uint8 a);

	EntityPtr createTexturedEntity(const string& assetTag, float x, float y, float width, float height, float tx, float ty, float w, float h, bool isCollidable);

	EntityPtr createAnimatedEntity(const string& path, float x, float y, float width, float height);

	EntityPtr createFromSerialization(const string& path);

	EntityPtr createPhysicsEntity(float x, float y, float width, float height);

	EntityPtr createScriptEntity(const string& path);

protected:
	SystemManagerPtr mSystemManager{ nullptr };
};

#endif // !__ENTITY_FACTORY_H__
