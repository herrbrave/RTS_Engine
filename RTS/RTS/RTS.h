#ifndef __RTS__
#define __RTS__

#include<stdlib.h>
#include<time.h>

#include<memory>
#include<vector>

#include"Animation.h"
#include"EntityFactory.h"
#include"Flowfield.h"
#include"Game.h"
#include"MapFactory.h"
#include"SoundControllerFactory.h"
#include"System.h"
#include"Ui.h"

class RTS : public Game {
private:
	SystemManagerPtr mSystemManager;

	EntityFactoryPtr mEntityFactory{ nullptr };
	EntityPtr mEntity{ nullptr };

	TileFactoryPtr mTileFactory{ nullptr };
	MapFactoryPtr mMapFactory{ nullptr };
	MapPtr mMap{ nullptr };

	WidgetFactoryPtr mWidgetFactory{ nullptr };

	SoundControllerFactoryPtr mSoundControllerFactory{ nullptr };

	Uint32 mLastTime{ 0 };

protected:
	void setup() override;
	void handleEvents() override;
	void update() override;
	void draw() override;
	void delay(Uint32 frameTime) override;
	void teardown() override;
};

class BulletState : public State {
public:
	BulletState(EntityPtr& entity, SystemManagerPtr& systemManager) {
		mEntity = entity;
		mSystemManager = systemManager;
	}

	void begin() override {
		mHasBegun = true;
	}

	void update() override {
		
		PhysicsComponentPtr physicsComponent = makeShared(mEntity->getComponentByType<PhysicsComponent>(ComponentType::PHYSICS_COMPONENT));
		if (physicsComponent->getPosition().y < 0 || physicsComponent->getPosition().y > 768) {
			EntitySystemPtr entitySystem = makeShared(mSystemManager->getSystemByType<EntitySystem>(SystemType::ENTITY));
			entitySystem->deregisterEntity(mEntity->id);
		}
	}

	void end() override {
		
	}

	bool hasBegun() override {
		return mHasBegun;
	}

	bool isEnded() override {
		return mDone;
	}

protected:
	EntityPtr mEntity;
	SystemManagerPtr mSystemManager;
	bool mHasBegun{ false };
	bool mDone{ false };
};


#endif // !__RTS__
