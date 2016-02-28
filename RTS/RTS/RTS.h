#ifndef __RTS__
#define __RTS__

#include<memory>

#include"EntityFactory.h"
#include"Game.h"
#include"Map.h"
#include"SystemManager.h"

class RTS : public Game {
private:
	std::unique_ptr<SystemManager> mSystemManager;

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
