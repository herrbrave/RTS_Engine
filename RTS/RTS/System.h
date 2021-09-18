#ifndef __SYSTEM_H__
#define __SYSTEM_H__

#include<chrono>
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
#include"Particle.h"
#include"Map.h"
#include"Physics.h"
#include"Script.h"
#include"ScriptComponent.h"
#include"SimpleDataStore.h"
#include"Sound.h"
#include"World.h"

enum class SystemType : Uint8 {
	ANIMATION = 0,
	ASSET = 1,
	ENTITY = 2,
	INPUT = 3,
	GRAPHICS = 4,
	PHYSICS = 5,
	SOUND = 6,
	LUA_SCRIPT = 7,
	PARTICLE = 8,
	DATA = 9,
	WORLD = 10,
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

class DataSystem;
typedef shared_ptr<DataSystem> DataSystemPtr;
typedef weak_ptr<DataSystem> WeakDataSystemPtr;

class EntitySystem;
typedef shared_ptr<EntitySystem> EntitySystemPtr;
typedef weak_ptr<EntitySystem> WeakEntitySystemPtr;

class InputSystem;
typedef shared_ptr<InputSystem> InputSystemPtr;
typedef weak_ptr<InputSystem> WeakInputSystemPtr;

class GraphicsSystem;
typedef shared_ptr<GraphicsSystem> GraphicsSystemPtr;
typedef weak_ptr<GraphicsSystem> WeakGraphicsSystemPtr;

class LuaScriptSystem;
typedef shared_ptr<LuaScriptSystem> LuaScriptSystemPtr;
typedef weak_ptr<LuaScriptSystem> WeakLuaScriptSystemPtr;

class ParticleSystem;
typedef shared_ptr<ParticleSystem> ParticleSystemPtr;
typedef weak_ptr<ParticleSystem> WeakParticleSystemPtr;

class PhysicsSystem;
typedef shared_ptr<PhysicsSystem> PhysicsSystemPtr;
typedef weak_ptr<PhysicsSystem> WeakPhysicsSystemPtr;

class SoundSystem;
typedef shared_ptr<SoundSystem> SoundSystemPtr;
typedef weak_ptr<SoundSystem> WeakSoundSystemPtr;

class WorldSystem;
typedef shared_ptr<WorldSystem> WorldSystemPtr;
typedef weak_ptr<WorldSystem> WeakWorldSystemPtr;


class SystemManager {
public:
	unordered_map<SystemType, SystemPtr> systems;

	SystemManager(GraphicsConfig* graphicsConfig);

	void update(Uint32 delta);

	void clear();

	template<class ClassType>
	shared_ptr<ClassType> getSystemByType(SystemType systemType) const {
		if (systems.find(systemType) == systems.end()) {
			throw "System does not exist.";
		}

		shared_ptr<System> systemPtr = systems.at(systemType);
		shared_ptr<ClassType> converted = static_pointer_cast<ClassType>(systemPtr);

		return converted;
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

	AnimationSetPtr loadAnimationSet(const string& path);

	void clear() override;

private:
	unordered_map<unsigned long, vector<AnimationHandlerPtr>> mAnimations;
};

class AssetSystem : public System {
public:
	AssetSystem(SystemManagerPtr systemManager) : System(SystemType::ASSET, systemManager) {
	}

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

class DataSystem : public System {
public:
	DataSystem(SystemManagerPtr systemManager) : System(SystemType::DATA, systemManager) {
	}

	void load(const string& path);
	void save(const string& path);
	void close(const string& path, bool saveFirst = true);

	const bool hasData(const string& dataStore, const string& key);

	const string& getData(const string& dataStore, const string& key);

	void putData(const string& dataStore, const string& key, const string& val);

	bool contains(const string& assetTag);

	void clear() override;
private:
	unordered_map<string, SimpleDataStorePtr> data;
};

class GraphicsSystem : public System {
public:
	GraphicsSystem(GraphicsConfig* graphicsConfig, SystemManagerPtr systemManager) : System(SystemType::GRAPHICS, systemManager) {
		mGraphicsConfig.reset(graphicsConfig);
		mGraphics.reset(GCC_NEW SDLGraphics(mGraphicsConfig, AssetVendorPtr(GCC_NEW AssetSystem::DefaultAssetVendor(static_pointer_cast<AssetSystem>(systemManager->systems.at(SystemType::ASSET))))));
		mCamera.reset(GCC_NEW Camera());
		mCamera->position.reset(GCC_NEW Vector2f(0, 0));
		mCamera->width = mGraphicsConfig->mWidth;
		mCamera->height = mGraphicsConfig->mHeight;
	}

	void initialize();

	void registerDrawable(const unsigned long, DrawablePtr drawable);
	void deregisterDrawable(const unsigned long);
	void draw();

	void drawTexture(const string& assetTag, float x, float y, float w, float h, float tx, float ty, float tw, float th, float angle, Uint8 r, Uint8 g, Uint8 b, Uint8 a);

	void drawTexture(TexturePtr texture, float x, float y, float w, float h, float tx, float ty, float tw, float th, float angle, Uint8 r, Uint8 g, Uint8 b, Uint8 a);

	void sortDrawableList(bool forceSort = false);

	void getDrawableById(unsigned long entityId, vector<WeakDrawablePtr>& drawables);

	WeakCameraPtr getCamera();

	void addTexture(const string& path, const string& assetTag);

	void addTexture(const string& assetTag, int width, int height);

	void clearTexture(const string& assetTag);

	void drawToTexture(const string& assetTag);

	void drawToScreen();

	void addFont(const string& path, const string&  assetTag, int fontsize);

	void clear() override;

	GraphicsConfigPtr getGraphicsConfig() {
		return this->mGraphicsConfig;
	}

	GraphicsPtr getGraphics() {
		return this->mGraphics;
	}

private:
	GraphicsConfigPtr mGraphicsConfig{ nullptr };
	GraphicsPtr mGraphics{ nullptr };
	CameraPtr mCamera;
	unordered_map<unsigned long, vector<DrawablePtr>> mDrawables;
	unordered_map<DrawablePtr, unsigned long> mReverseLookup;
	std::list<DrawablePtr> mDrawableList;
	bool sortDrawables = true;
};

class EntitySystem : public System {
public:
	EntitySystem(SystemManagerPtr systemManager) : System(SystemType::ENTITY, systemManager) {
		EventDelegate destroyEntityDelegate([this](const EventData& eventData) {
			EntityDestroyedEventData data = dynamic_cast<const EntityDestroyedEventData&>(eventData);
			
			auto val = mEntityMap.find(data.getEntityId());
			if (val != mEntityMap.end()) {
				mEntityMap.erase(val);
			}
		});

		EventListenerDelegate destroyEntityListener(destroyEntityDelegate);
		EventManager::getInstance().addDelegate(destroyEntityListener, EventType::ENTITY_DESTROYED);
	}

	void addEntity(EntityPtr entity);

	EntityPtr getEntityById(unsigned long id);

	void deregisterEntity(unsigned long id);

	void getAllEntities(std::vector<EntityPtr>& entity);
	
	void update(Uint32 delta);

	void clear() override;

	class DefaultEntityVendor : public EntityVendor {
	public:
		DefaultEntityVendor(EntitySystemPtr entitySystem) { mEntitySystem = entitySystem; }
		EntityPtr getEntityById(unsigned long entityId) override;
	private:
		EntitySystemPtr mEntitySystem;
	};

private:
	unordered_map<unsigned long, EntityPtr> mEntityMap;
};

class LuaScriptSystem : public System {
public:
	LuaScriptSystem(SystemManagerPtr systemManager) : System(SystemType::LUA_SCRIPT, systemManager) {

		EventDelegate scriptLoadedDelegate([this](const EventData& eventData) {
			ScriptLoadedData data = dynamic_cast<const ScriptLoadedData&>(eventData);
			registerLuaScript(data.id, data.script);
		});

		EventListenerDelegate scriptLoadedListener(scriptLoadedDelegate);
		EventManager::getInstance().addDelegate(scriptLoadedListener, EventType::SCRIPT_LOADED);

		EventDelegate destroyEntityDelegate([this](const EventData& eventData) {
			EntityDestroyedEventData data = dynamic_cast<const EntityDestroyedEventData&>(eventData);

			deregisterLuaScript(data.getEntityId());
		});

		EventListenerDelegate destroyEntityListener(destroyEntityDelegate);
		EventManager::getInstance().addDelegate(destroyEntityListener, EventType::ENTITY_DESTROYED);
	}

	LuaScripts getLuaScripts();

	void registerLuaScript(unsigned long id, const LuaScriptPtr& luaScript);

	void deregisterLuaScript(unsigned long id);

	void update(Uint32 delta);

	void clear() override;

private:
	LuaScripts mLuaScripts;
};

class ParticleSystem : public System {
public:

	ParticleSystem(SystemManagerPtr systemManager) : System(SystemType::PARTICLE, systemManager) {

	}

	void registerParticleEmitter(unsigned long id, const ParticleEmitterPtr& emitter);

	void deregisterParticleEmitter(unsigned long id);

	void update(Uint32 delta);

	void clear() override;
private:
	unordered_map<unsigned long, ParticleEmitterPtr> particleEmitters;
};

class PhysicsSystem : public System {
public:
	QuadtreePtr quadTree{ nullptr };

	PhysicsSystem(SystemManagerPtr systemManager) : System(SystemType::PHYSICS, systemManager) {
		GraphicsSystemPtr graphicsSystem = systemManager->getSystemByType<GraphicsSystem>(SystemType::GRAPHICS);
		GraphicsConfigPtr graphicsConfig = graphicsSystem->getGraphicsConfig();
		this->setWorldSize(graphicsConfig->mWidth, graphicsConfig->mHeight);

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

	void setWorldSize(int width, int height);

	void moveAndCollide(const unsigned int id, const Vector2f& delta);

	Vector2fPtr moveAndSlide(const unsigned int id, const Vector2f& delta);

	void update(Uint32 delta);

	void clear() override;

	unordered_map<unsigned long, BodyPtr> mBodies;
private:
	vector<PhysicsBehaviorPtr> mBehaviors;

	Vector2f handleCollision(const Vector2f& delta, BodyPtr body, bool slide);
	Sweep sweep(const Vector2f& position, const Vector2f& delta, BodyPtr body);
};

class DefaultMouseMovementHandler : public MouseMovementHandler {
public:
	DefaultMouseMovementHandler(SystemManagerPtr systemManager) {
		mSystemManager = systemManager;
	}

	bool checkForMouseOver(unsigned long id, const Vector2f& position) override;

	bool translateToRelative(unsigned long id, const Vector2f& position, Vector2f& relative) override;

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

	bool contains(unsigned long id);

	WeakInputListenerPtr at(unsigned long id);

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
		if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) < 0) {
			string error("Error initializing SDL_Mixer: ");
			error.append(SDL_GetError());
			throw std::exception(error.c_str());
		}
	}

	void loadSound(const string& path, const string& assetTag, SoundType soundType);

	void playSound(const string& assetTag, int channel, int loop = -1);

	void playMusic(const string& assetTag, int loop = -1);

	void stopSound(const string& assetTag, int channel);

	void stopMusic(const string& assetTag);

	void pause(const string& assetTag);

	SoundControllerPtr createController(const string& assetTag, SoundType soundType);

	void clear() override {/* no op */}
};

class WorldSystem : public System {
public:
	WorldSystem(SystemManagerPtr systemManager);

	void loadWorld(string& path);

	void destroyWorld();

	void addWorld(WorldPtr world);

	WorldPtr getWorld();

	void clear() override {/* no op */ }

private:
	void registerEntityAndComponents(EntityPtr entity);
	WorldPtr world{ nullptr };
};

#endif // !__SYSTEM_H__
