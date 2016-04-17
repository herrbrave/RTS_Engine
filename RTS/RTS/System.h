#ifndef __SYSTEM_H__
#define __SYSTEM_H__

#include<functional>
#include<memory>
#include<string>
#include<unordered_map>

#include"Asset.h"
#include"Entity.h"
#include"Input.h"
#include"Graphics.h"
#include"Physics.h"

enum class SystemType {
	ASSET = 1,
	ENTITY = 2,
	INPUT = 3,
	GRAPHICS = 4,
	PHYSICS = 5
};

class System;
class SystemManager {
public:
	std::unordered_map<SystemType, System*> systems;

	SystemManager(GraphicsConfig* graphicsConfig);
};

class System {
public:
	SystemType type;
	System(SystemType type, SystemManager* systemManager) { 
		this->type = type; 
		this->mSystemManager = systemManager;
	}

	virtual void clear() = 0;

protected:
	SystemManager* mSystemManager;
};

class AssetSystem : public System {
public:
	AssetSystem(SystemManager* systemManager) : System(SystemType::ASSET, systemManager) {}

	void registerAsset(Asset* asset) {
		mAssets.emplace(asset->tag, asset);
	}

	void deregisterAsset(const std::string& assetTag) {
		mAssets.erase(mAssets.find(assetTag));
	}

	Asset* getAsset(const std::string& assetTag) {
		return mAssets[assetTag];
	}

	bool contains(const std::string& assetTag) {
		return (mAssets.find(assetTag) != mAssets.end());
	}

	void clear() override;

	class DefaultAssetVendor : public AssetVendor {
	public:
		DefaultAssetVendor(AssetSystem* assetSystem) {
			mAssetSystem = assetSystem;
		}
		Asset* getAsset(const std::string& tag) override;
	private:
		AssetSystem* mAssetSystem;
	};

private:
	std::unordered_map<std::string, Asset*> mAssets;
};

class GraphicsSystem : public System {
public:
	GraphicsSystem(GraphicsConfig* graphisConfig, SystemManager* systemManager) : System(SystemType::GRAPHICS, systemManager) {
		mGraphicsConfig.reset(std::move(graphisConfig));
		mGraphics.reset(new SDLGraphics(mGraphicsConfig.get(), new AssetSystem::DefaultAssetVendor(reinterpret_cast<AssetSystem*>(systemManager->systems.at(SystemType::ASSET)))));
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

	void addTexture(const std::string& path, const std::string& assetTag);

	void createTextSurface(const std::string& text, const std::string& assetTag, Uint8 r, Uint8 g, Uint8 b, Uint8 a);

	void clear() override;
private:
	std::unique_ptr<GraphicsConfig> mGraphicsConfig{ nullptr };
	std::unique_ptr<Graphics> mGraphics{ nullptr };
	std::unique_ptr<Camera> mCamera;
	std::unordered_map<unsigned long, Drawable*> mDrawables;
};

class EntitySystem : public System {
public:
	EntitySystem(SystemManager* systemManager) : System(SystemType::ENTITY, systemManager) {}

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
	std::unordered_map<unsigned long, Entity*> mEntityMap;
};

class DefaultPhysicsNotifier : public PhysicsNotifier {
public:

	DefaultPhysicsNotifier(SystemManager* systemManager) {
		this->systemManager = systemManager;
	}

	void notifyPositionSet(unsigned long id) override;
	void notifyColliderUpdate(unsigned long id) override;

private:
	SystemManager* systemManager;
};

class PhysicsSystem : public System {
public:
	std::unique_ptr<Quadtree> quadTree{ nullptr };
	std::unique_ptr<PhysicsNotifier> physicsNotifier{ nullptr };

	PhysicsSystem(SystemManager* systemManager) : System(SystemType::PHYSICS, systemManager) {
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
	std::unordered_map<unsigned long, Body*> mBodies;
	void handleCollision(Body* incidentBody, Body* otherBody);
};

class DefaultMouseMovementHandler : public MouseMovementHandler {
public:
	DefaultMouseMovementHandler(SystemManager* systemManager) {
		mSystemManager = systemManager;
	}

	bool checkForMouseOver(unsigned long id, const vector2f& position) override;

private:
	SystemManager* mSystemManager;
};

class InputSystem : public System {
public:
	InputSystem(SystemManager* systemManager) : System(SystemType::INPUT, systemManager) {
		mMouseMovementHandler.reset(new DefaultMouseMovementHandler(systemManager));
	}

	void registerEventListener(InputListener* inputListener);
	void deregisterEventListener(unsigned long id);

	void handleEvent(const SDL_Event& evt);

	void clear() override;

private:
	std::unordered_map<unsigned long, InputListener*> mListeners;
	std::unique_ptr<MouseMovementHandler> mMouseMovementHandler{ nullptr };
};


void getEntityPosition(vector2f* vector, Entity* entity, SystemManager* systemManager);

void getPositionById(vector2f* vector, unsigned long id, SystemManager& systemManager);

Entity* getEntityById(unsigned long entityId, SystemManager* systemManager);

bool assetExists(const std::string& assetTag, SystemManager* systemManager);

Asset* getAsset(const std::string& assetTag, SystemManager* systemManager);

void updatePhysicsSystem(Uint32 ticks, SystemManager* systemManager);

void drawGraphicsSystem(SystemManager* systemManager);

void handleInput(const SDL_Event& event, SystemManager* systemManager);

Drawable* getDrawableById(unsigned long drawableId, SystemManager* systemManager);

Body* getBodyById(unsigned long bodyId, SystemManager* systemManager); 

void destroyEntity(unsigned long entityId, SystemManager* systemManager);

void destroyAllEntities(SystemManager* systemManager);

#endif // !__SYSTEM_H__
