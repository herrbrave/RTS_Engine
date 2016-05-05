#ifndef __SYSTEM_H__
#define __SYSTEM_H__

#include<functional>
#include<memory>
#include<string>
#include<unordered_map>

#include"Animation.h"
#include"Asset.h"
#include"Entity.h"
#include"Input.h"
#include"Graphics.h"
#include"Physics.h"
#include"Sound.h"

enum class SystemType {
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
class SystemManager;
class AnimationSystem;
class AssetSystem;
class EntitySystem;
class InputSystem;
class GraphicsSystem;
class PhysicsSystem;
class SoundSystem;

/* Smart pointers aliases. */
using std::shared_ptr;
using std::vector;
using std::unordered_map;
using std::string;
using std::static_pointer_cast;

typedef shared_ptr<System> SystemPtr;
typedef shared_ptr<SystemManager> SystemManagerPtr;
typedef shared_ptr<AnimationSystem> AnimationSystemPtr;
typedef shared_ptr<AssetSystem> AssetSystemPtr;
typedef shared_ptr<EntitySystem> EntitySystemPtr;
typedef shared_ptr<InputSystem> InputSystemPtr;
typedef shared_ptr<GraphicsSystem> GraphicsSystemPtr;
typedef shared_ptr<PhysicsSystem> PhysicsSystemPtr;
typedef shared_ptr<SoundSystem> SoundSystemPtr;


class SystemManager {
public:
	unordered_map<SystemType, SystemPtr> systems;

	SystemManager(GraphicsConfig* graphicsConfig);
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
	AnimationSystem(SystemManagerPtr systemManager) : System(SystemType::ANIMATION, systemManager) {}

	void update(Uint32 delta);

	void registerAnimation(unsigned long id, AnimationHandler* animationHandler);

	void deregisterAnimation(unsigned long id);

	AnimationSet* createAnimationSet(string path);

	void clear() override;

private:
	unordered_map<unsigned long, AnimationHandler*> mAnimations;
};

class AssetSystem : public System {
public:
	AssetSystem(SystemManagerPtr systemManager) : System(SystemType::ASSET, systemManager) {}

	void registerAsset(Asset* asset) {
		mAssets.emplace(asset->tag, asset);
	}

	void deregisterAsset(const string& assetTag) {
		mAssets.erase(mAssets.find(assetTag));
	}

	Asset* getAsset(const string& assetTag) {
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
		Asset* getAsset(const string& tag) override;
	private:
		AssetSystemPtr mAssetSystem;
	};

private:
	unordered_map<string, Asset*> mAssets;
};

class GraphicsSystem : public System {
public:
	GraphicsSystem(GraphicsConfig* graphisConfig, SystemManagerPtr systemManager) : System(SystemType::GRAPHICS, systemManager) {
		mGraphicsConfig.reset(std::move(graphisConfig));
		mGraphics.reset(new SDLGraphics(mGraphicsConfig.get(), new AssetSystem::DefaultAssetVendor(static_pointer_cast<AssetSystem>(systemManager->systems.at(SystemType::ASSET)))));
		mCamera.reset(new Camera());
		mCamera->position.reset(new vector2f(0, 0));
		mCamera->width = mGraphicsConfig->mWidth;
		mCamera->height = mGraphicsConfig->mHeight;
	}

	void registerDrawable(const unsigned long, Drawable* drawable);
	void deregisterDrawable(const unsigned long);
	void draw();

	Drawable* getDrawableById(unsigned long entityId);

	Camera* getCamera();

	void addTexture(const string& path, const string& assetTag);

	void createTextSurface(const string& text, const string& assetTag, Uint8 r, Uint8 g, Uint8 b, Uint8 a);

	void clear() override;
private:
	std::unique_ptr<GraphicsConfig> mGraphicsConfig{ nullptr };
	std::unique_ptr<Graphics> mGraphics{ nullptr };
	std::unique_ptr<Camera> mCamera;
	unordered_map<unsigned long, Drawable*> mDrawables;
};

class EntitySystem : public System {
public:
	EntitySystem(SystemManagerPtr systemManager) : System(SystemType::ENTITY, systemManager) {}

	void registerEntity(Entity* entity);
	Entity* getEntityById(unsigned long id);
	void deregisterEntity(unsigned long id);
	void getAllEntities(std::vector<Entity*>& entity);

	void clear() override;

	class DefaultEntityVendor : public EntityVendor {
	public:
		DefaultEntityVendor(EntitySystem* entitySystem) { mEntitySystem = entitySystem; }
		Entity* getEntityById(unsigned long entityId) override;
	private:
		EntitySystem* mEntitySystem;
	};

private:
	unordered_map<unsigned long, Entity*> mEntityMap;
};

class DefaultPhysicsNotifier : public PhysicsNotifier {
public:

	DefaultPhysicsNotifier(SystemManagerPtr systemManager) {
		this->systemManager = systemManager;
	}

	void notifyPositionSet(unsigned long id) override;
	void notifyColliderUpdate(unsigned long id) override;

private:
	SystemManagerPtr systemManager;
};

class PhysicsSystem : public System {
public:
	std::unique_ptr<Quadtree> quadTree{ nullptr };
	std::unique_ptr<PhysicsNotifier> physicsNotifier{ nullptr };

	PhysicsSystem(SystemManagerPtr systemManager) : System(SystemType::PHYSICS, systemManager) {
		quadTree.reset(new Quadtree(512, 384, 1024, 768));
		physicsNotifier.reset(new DefaultPhysicsNotifier(systemManager));
	}

	~PhysicsSystem() = default;

	Body* getBody(const unsigned long id);

	void registerBody(const unsigned long id, Body* body);
	void deregisterBody(const unsigned long id);

	void update(Uint32 delta);

	void clear() override;

private:
	unordered_map<unsigned long, Body*> mBodies;
	float sweptAABB(Body& incidentBody, Body& otherBody, const vector2f& velocity, vector2f& normal);
};

class DefaultMouseMovementHandler : public MouseMovementHandler {
public:
	DefaultMouseMovementHandler(SystemManagerPtr systemManager) {
		mSystemManager = systemManager;
	}

	bool checkForMouseOver(unsigned long id, const vector2f& position) override;

private:
	SystemManagerPtr mSystemManager;
};

class InputSystem : public System {
public:
	InputSystem(SystemManagerPtr systemManager) : System(SystemType::INPUT, systemManager) {
		mMouseMovementHandler.reset(new DefaultMouseMovementHandler(systemManager));
	}

	void registerEventListener(InputListener* inputListener);
	void deregisterEventListener(unsigned long id);

	void handleEvent(const SDL_Event& evt);

	void clear() override;

private:
	unordered_map<unsigned long, InputListener*> mListeners;
	std::unique_ptr<MouseMovementHandler> mMouseMovementHandler{ nullptr };
};

class DefaultSoundController : public SoundController {
public:
	DefaultSoundController(Sound* sound, SystemManager* systemManager) : SoundController(sound) {
		mSystemManager = systemManager;
	}

	void play(int loop = 0) override;
	void pause() override;
	void stop() override;
private:
	SystemManager* mSystemManager;
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

	SoundController* createController(const string& assetTag, SoundType soundType);

	void clear() override {/* no op */}
};

void getEntityPosition(vector2f* vector, Entity* entity, SystemManager* systemManager);

void getPositionById(vector2f* vector, unsigned long id, SystemManager& systemManager);

Entity* getEntityById(unsigned long entityId, SystemManager* systemManager);

bool assetExists(const string& assetTag, SystemManager* systemManager);

Asset* getAsset(const string& assetTag, SystemManager* systemManager);

void updatePhysicsSystem(Uint32 ticks, SystemManager* systemManager);

void drawGraphicsSystem(SystemManager* systemManager);

void handleInput(const SDL_Event& event, SystemManager* systemManager);

Drawable* getDrawableById(unsigned long drawableId, SystemManager* systemManager);

Body* getBodyById(unsigned long bodyId, SystemManager* systemManager); 

void destroyEntity(unsigned long entityId, SystemManager* systemManager);

void destroyAllEntities(SystemManager* systemManager);

#endif // !__SYSTEM_H__
