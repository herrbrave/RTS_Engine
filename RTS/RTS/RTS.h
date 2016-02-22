#ifndef __RTS__
#define __RTS__

#include<functional>
#include<memory>
#include<string>
#include<vector>
#include<unordered_set>
#include<queue>
#include<unordered_map>

#include"Game.h"
#include"Graphics.h"
#include"MoveState.h"
#include"PathState.h"
#include"SDL_Helpers.h"
#include"State.h"

class RTS : public Game {
private:
	std::unique_ptr<PhysicsSystem> mPhysicsSystem{ nullptr };
	std::unique_ptr<GraphicsSystem> mGraphicsSystem{ nullptr };

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
