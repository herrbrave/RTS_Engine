#ifndef __RTS__
#define __RTS__

#include<memory>

#include"Game.h"
#include"Map.h"

class RTS : public Game {
private:
	std::unique_ptr<AssetSystem> mAssetSystem{ nullptr };
	std::unique_ptr<EntitySystem> mEntitySystem{ nullptr };
	std::unique_ptr<PhysicsSystem> mPhysicsSystem{ nullptr };
	std::unique_ptr<GraphicsSystem> mGraphicsSystem{ nullptr };

	std::unique_ptr<EntityFactory> mEntityFactory{ nullptr };

	std::shared_ptr<TTF_Font> mFont{ nullptr };

	Uint32 mLastTime{ 0 };

protected:
	virtual void setup();
	virtual void handleEvents();
	virtual void update();
	virtual void draw();
	virtual void delay(Uint32 frameTime);
	virtual void teardown();
};


#endif // !__RTS__
