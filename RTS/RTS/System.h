#ifndef __SYSTEM_H__
#define __SYSTEM_H__

#include<functional>
#include<memory>
#include<string>
#include<unordered_map>

#include"Animation.h"
#include"Asset.h"
#include"Entity.h"
#include"Event.h"
#include"Input.h"
#include"Graphics.h"
#include"Physics.h"
#include"Sound.h"

enum class SystemType : Uint8 {
	ANIMATION = 0,
	ASSET = 1,
	ENTITY = 2,
	INPUT = 3,
	GRAPHICS = 4,
	PHYSICS = 5,
	SOUND = 6,
};

/* Early class definitions. */
class System;
typedef shared_ptr<System> SystemPtr;
typedef weak_ptr<System> WeakSystemPtr;

class SystemManager;
typedef shared_ptr<SystemManager> SystemManagerPtr;
typedef weak_ptr<SystemManager> WeakSystemManagerPtr;

class AnimationSystem;
typedef shared_ptr<AnimationSystem> AnimationSystemPtr;
typedef weak_ptr<AnimationSystem> WeakAnimationSystemPtr;

class AssetSystem;
typedef shared_ptr<AssetSystem> AssetSystemPtr;
typedef weak_ptr<AssetSystem> WeakAssetSystemPtr;

class EntitySystem;
typedef shared_ptr<EntitySystem> EntitySystemPtr;
typedef weak_ptr<EntitySystem> WeakEntitySystemPtr;

class InputSystem;
typedef shared_ptr<InputSystem> InputSystemPtr;
typedef weak_ptr<InputSystem> WeakInputSystemPtr;

class GraphicsSystem;
typedef shared_ptr<GraphicsSystem> GraphicsSystemPtr;
typedef weak_ptr<GraphicsSystem> WeakGraphicsSystemPtr;

class PhysicsSystem;
typedef shared_ptr<PhysicsSystem> PhysicsSystemPtr;
typedef weak_ptr<PhysicsSystem> WeakPhysicsSystemPtr;

class SoundSystem;
typedef shared_ptr<SoundSystem> SoundSystemPtr;
typedef weak_ptr<SoundSystem> WeakSoundSystemPtr;


class SystemManager {
public:
	unordered_map<SystemType, SystemPtr> systems;

	SystemManager(GraphicsConfig* graphicsConfig);

	template<class ClassType>
	weak_ptr<ClassType> getSystemByType(SystemType systemType) const {
		if (systems.find(systemType) == systems.end()) {
			return weak_ptr<ClassType>();
		}

		shared_ptr<System> systemPtr = systems.at(systemType);
		shared_ptr<ClassType> converted = static_pointer_cast<ClassType>(systemPtr);

		return weak_ptr<ClassType>(converted);
	}
};

class System {
public:
	SystemType type;
	System(SystemType type, SystemManagerPtr systemManager) {
		this->type = type; 
		this->mSystemManager = systemManager;
	}

	virtual void clear() = 0;

protected:
	SystemManagerPtr mSystemManager;
};

class AnimationSystem : public System {
public:
	AnimationSystem(SystemManagerPtr systemManager) : System(SystemType::ANIMATION, systemManager) {
		EventDelegate destroyEntityDelegate([this](const EventData& eventData) {
			EntityDestroyedEventData data = dynamic_cast<const EntityDestroyedEventData&>(eventData);

			deregisterAnimation(data.getEntityId());
		});

		EventListenerDelegate destroyEntityListener(destroyEntityDelegate);
		EventManager::getInstance().addDelegate(destroyEntityListener, EventType::ENTITY_DESTROYED);
	}

	void update(Uint32 delta);

	void registerAnimation(unsigned long id, AnimationHandlerPtr animationHandler);

	void deregisterAnimation(unsigned long id);

	AnimationSetPtr createAnimationSet(const string& path);

	void clear() override;

private:
	unordered_map<unsigned long, AnimationHandlerPtr> mAnimations;
};

class AssetSystem : public System {
public:
	AssetSystem(SystemManagerPtr systemManager) : System(SystemType::ASSET, systemManager) {}

	void registerAsset(AssetPtr asset) {
		mAssets.emplace(asset->tag, asset);
	}

	void deregisterAsset(const string& assetTag) {
		mAssets.erase(mAssets.find(assetTag));
	}

	AssetPtr getAsset(const string& assetTag) {
		return mAssets[assetTag];
	}

	bool contains(const string& assetTag) {
		return (mAssets.find(assetTag) != mAssets.end());
	}

	void clear() override;

	class DefaultAssetVendor : public AssetVendor {
	public:
		DefaultAssetVendor(AssetSystemPtr assetSystem) {
			mAssetSystem = assetSystem;
		}
		AssetPtr getAsset(const string& tag) override;
	private:
		AssetSystemPtr mAssetSystem;
	};

private:
	unordered_map<string, AssetPtr> mAssets;
};

class GraphicsSystem : public System {
public:
	GraphicsSystem(GraphicsConfigPtr graphisConfig, SystemManagerPtr systemManager) : System(SystemType::GRAPHICS, systemManager) {
		mGraphicsConfig = std::move(graphisConfig);
		mGraphics.reset(GCC_NEW SDLGraphics(mGraphicsConfig, AssetVendorPtr(GCC_NEW AssetSystem::DefaultAssetVendor(static_pointer_cast<AssetSystem>(systemManager->systems.at(SystemType::ASSET))))));
		mCamera.reset(GCC_NEW Camera());
		mCamera->position.reset(GCC_NEW Vector2f(0, 0));
		mCamera->width = mGraphicsConfig->mWidth;
		mCamera->height = mGraphicsConfig->mHeight;

		EventDelegate zOrderChangeDelegate([this](const EventData& eventData){
			sortDrawableList();
		});
		EventListenerDelegate zOrderChanged(zOrderChangeDelegate);
		EventManager::getInstance().addDelegate(zOrderChanged, EventType::ENTITY_ZORDER_SET);

		EventDelegate destroyEntityDelegate([this](const EventData& eventData) {
			EntityDestroyedEventData data = dynamic_cast<const EntityDestroyedEventData&>(eventData);

			deregisterDrawable(data.getEntityId());
		});

		EventListenerDelegate destroyEntityListener(destroyEntityDelegate);
		EventManager::getInstance().addDelegate(destroyEntityListener, EventType::ENTITY_DESTROYED);
	}

	void registerDrawable(const unsigned long, DrawablePtr drawable);
	void deregisterDrawable(const unsigned long);
	void draw();

	void sortDrawableList();

	WeakDrawablePtr getDrawableById(unsigned long entityId);

	WeakCameraPtr getCamera();

	void addTexture(const string& path, const string& assetTag);

	void createTextSurface(const string& text, const string& assetTag, Uint8 r, Uint8 g, Uint8 b, Uint8 a);

	void clear() override;
private:
	GraphicsConfigPtr mGraphicsConfig{ nullptr };
	GraphicsPtr mGraphics{ nullptr };
	CameraPtr mCamera;
	unordered_map<unsigned long, DrawablePtr> mDrawables;
	unordered_map<DrawablePtr, unsigned long> mReverseLookup;
	std::list<DrawablePtr> mDrawableList;
};

class EntitySystem : public System {
public:
	EntitySystem(SystemManagerPtr systemManager) : System(SystemType::ENTITY, systemManager) {
		EventDelegate destroyEntityDelegate([this](const EventData& eventData) {
			EntityDestroyedEventData data = dynamic_cast<const EntityDestroyedEventData&>(eventData);

			mEntityMap.erase(mEntityMap.find(data.getEntityId()));
		});

		EventListenerDelegate destroyEntityListener(destroyEntityDelegate);
		EventManager::getInstance().addDelegate(destroyEntityListener, EventType::ENTITY_DESTROYED);
	}

	void addEntity(EntityPtr entity);
	WeakEntityPtr getEntityById(unsigned long id);
	void deregisterEntity(unsigned long id);
	void getAllEntities(std::vector<EntityPtr>& entity);
	
	void update(Uint32 delta);

	void clear() override;

	class DefaultEntityVendor : public EntityVendor {
	public:
		DefaultEntityVendor(EntitySystemPtr entitySystem) { mEntitySystem = entitySystem; }
		WeakEntityPtr getEntityById(unsigned long entityId) override;
	private:
		EntitySystemPtr mEntitySystem;
	};

private:
	unordered_map<unsigned long, EntityPtr> mEntityMap;
}; 

class PhysicsSystem : public System {
public:
	QuadtreePtr quadTree{ nullptr };

	PhysicsSystem(SystemManagerPtr systemManager) : System(SystemType::PHYSICS, systemManager) {
		quadTree.reset(GCC_NEW Quadtree(512, 384, 1024, 768));

		EventListenerDelegate positionChangeListener([this](const EventData& eventData) {
			EventData& nonConstEventData = const_cast<EventData&>(eventData);
			EntityPositionSetEventData& positionData = reinterpret_cast<EntityPositionSetEventData&>(nonConstEventData);

			BodyPtr body = makeShared(getBody(positionData.getEntityId()));
			quadTree->removeBody(body);
			if (body->collider != nullptr) {
				quadTree->addBody(body);
			}
		});
		EventManager::getInstance().addDelegate(positionChangeListener, EventType::ENTITY_POSITION_SET);

		EventListenerDelegate collisionChangeListener([this](const EventData& eventData) {
			EventData& nonConstEventData = const_cast<EventData&>(eventData);
			EntityCollisionSetEventData& collisionData = reinterpret_cast<EntityCollisionSetEventData&>(nonConstEventData);

			BodyPtr body = makeShared(getBody(collisionData.getEntityId()));
			quadTree->removeBody(body);
			if (body->collider != nullptr) {
				quadTree->addBody(body);
			}
		});
		EventManager::getInstance().addDelegate(collisionChangeListener, EventType::ENTITY_COLLISION_SET);

		EventDelegate destroyEntityDelegate([this](const EventData& eventData) {
			EntityDestroyedEventData data = dynamic_cast<const EntityDestroyedEventData&>(eventData);

			deregisterBody(data.getEntityId());
		});

		EventListenerDelegate destroyEntityListener(destroyEntityDelegate);
		EventManager::getInstance().addDelegate(destroyEntityListener, EventType::ENTITY_DESTROYED);

		mBehaviors.push_back(PhysicsBehaviorPtr(new BasicBehavior()));
	}

	~PhysicsSystem() = default;

	WeakBodyPtr getBody(const unsigned long id);

	void registerBody(const unsigned long id, BodyPtr body);
	void deregisterBody(const unsigned long id);

	void update(Uint32 delta);

	void clear() override;

private:
	unordered_map<unsigned long, BodyPtr> mBodies;
	vector<PhysicsBehaviorPtr> mBehaviors;
};

class DefaultMouseMovementHandler : public MouseMovementHandler {
public:
	DefaultMouseMovementHandler(SystemManagerPtr systemManager) {
		mSystemManager = systemManager;
	}

	bool checkForMouseOver(unsigned long id, const Vector2f& position) override;

private:
	SystemManagerPtr mSystemManager;
};

class InputSystem : public System {
public:
	InputSystem(SystemManagerPtr systemManager) : System(SystemType::INPUT, systemManager) {
		mMouseMovementHandler.reset(GCC_NEW DefaultMouseMovementHandler(systemManager));

		EventDelegate destroyEntityDelegate([this](const EventData& eventData) {
			EntityDestroyedEventData data = dynamic_cast<const EntityDestroyedEventData&>(eventData);

			deregisterEventListener(data.getEntityId());
		});

		EventListenerDelegate destroyEntityListener(destroyEntityDelegate);
		EventManager::getInstance().addDelegate(destroyEntityListener, EventType::ENTITY_DESTROYED);
	}

	void registerEventListener(InputListenerPtr inputListener);
	void deregisterEventListener(unsigned long id);

	void handleEvent(const SDL_Event& evt);

	void clear() override;

private:
	unordered_map<unsigned long, InputListenerPtr> mListeners;
	MouseMovementHandlerPtr mMouseMovementHandler{ nullptr };
};

class DefaultSoundController : public SoundController {
public:
	DefaultSoundController(SoundPtr sound, SystemManagerPtr systemManager) : SoundController(sound) {
		mSystemManager = systemManager;
	}

	void play(int loop = 0) override;
	void pause() override;
	void stop() override;
private:
	SystemManagerPtr mSystemManager;
};

class SoundSystem : public System {
public:
	SoundSystem(SystemManagerPtr systemManager) : System(SystemType::SOUND, systemManager) {
		if (!Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) < 0) {
			string error("Error initializing SDL_Mixer: ");
			error.append(SDL_GetError());
			throw std::exception(error.c_str());
		}
	}

	void loadSound(const string& path, const string& assetTag, SoundType soundType);

	SoundControllerPtr createController(const string& assetTag, SoundType soundType);

	void clear() override {/* no op */}
};

#endif // !__SYSTEM_H__
