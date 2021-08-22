#ifndef __ENTITY_FACTORY_H__
#define __ENTITY_FACTORY_H__

#include"Entity.h"
#include"Map.h"
#include"Particle.h"
#include"Serializer.h"
#include"System.h"
#include"ScriptComponent.h"

class EntityFactory;
typedef shared_ptr<EntityFactory> EntityFactoryPtr;
typedef weak_ptr<EntityFactory> WeakEntityFactoryPtr;


void applyDrawable(SystemManagerPtr systemManager, unsigned long entityId, float x, float y, float width, float height, Uint8 r, Uint8 g, Uint8 b, Uint8 a);
void applyDrawable(SystemManagerPtr systemManager, unsigned long entityId, const string& texturePath, float width, float height, float tx, float ty, float w, float h);
void applyDrawable(SystemManagerPtr systemManager, unsigned long entityId, TexturePtr texture, float width, float height);
void applyPhysics(SystemManagerPtr systemManager, unsigned long entityId, float x, float y, float w, float h);
void applyPhysics(SystemManagerPtr systemManager, unsigned long entityId, float x, float y, float w, float h, ColliderShapePtr collider);
void applyInput(SystemManagerPtr systemManager, unsigned long entityId, Input input, std::function<bool(EventPtr)>& callback);
void applyAnimation(SystemManagerPtr systemManager, unsigned long entityId, const string& path, int width, int height);
void applyAnimation(SystemManagerPtr systemManager, unsigned long entityId, AnimationSetPtr animationSet, int width, int height);
void applyScript(SystemManagerPtr systemManager, unsigned long entityId, const string& script);
void applyParticle(SystemManagerPtr systemManager, unsigned long entityId, const string& animation, Uint32 particleLifeMillis, const Vector2f& gravity, float particleWidth, float particleHeight, float spreadDist, float particleSpeed);


class EntityFactory {
public:
	EntityFactory(SystemManagerPtr systemManager);

	EntityPtr createDefault(float x, float y, float width, float height, Uint8 r, Uint8 g, Uint8 b, Uint8 a);

	EntityPtr createTexturedEntity(const string& assetTag, float x, float y, float width, float height, float tx, float ty, float w, float h, bool isCollidable);

	EntityPtr createAnimatedEntity(const string& path, float x, float y, float width, float height);

	EntityPtr createFromSerialization(const string& path);

	EntityPtr createPhysicsEntity(float x, float y, float width, float height, bool isCollidable = false);

	EntityPtr createScriptEntity(const string& path);

protected:
	SystemManagerPtr mSystemManager{ nullptr };
};

#endif // !__ENTITY_FACTORY_H__
