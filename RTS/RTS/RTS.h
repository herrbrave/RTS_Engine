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
#include"Tower.h"



static int const MAP_WIDTH = 10;
static int const MAP_HEIGHT = 10;
static const std::vector<TileType> MAP {
	TREES, TREES, TREES, DIRT, GRASS, WATER, WATER, WATER, WATER, WATER,
	TREES, TREES, DIRT, GRASS, GRASS, GRASS, GRASS, WATER, WATER, WATER,
	TREES, TREES, DIRT, GRASS, GRASS, GRASS, GRASS, WATER, WATER, WATER,
	TREES, DIRT, TREES, GRASS, GRASS, GRASS, GRASS, GRASS, WATER, WATER,
	DIRT, DIRT, GRASS, GRASS, GRASS, GRASS, GRASS, GRASS, GRASS, WATER,
	DIRT, DIRT, GRASS, GRASS, GRASS, GRASS, GRASS, GRASS, GRASS, WATER,
	GRASS, DIRT, DIRT, GRASS, GRASS, GRASS, GRASS, GRASS, GRASS, GRASS,
	GRASS, GRASS, DIRT, GRASS, GRASS, GRASS, GRASS, GRASS, GRASS, GRASS,
	GRASS, GRASS, DIRT, GRASS, GRASS, GRASS, GRASS, GRASS, GRASS, GRASS,
	GRASS, GRASS, DIRT, GRASS, GRASS, GRASS, GRASS, GRASS, GRASS, GRASS
};

class RTS : public Game {
private:
	std::unique_ptr<GraphicsSystem> mGraphicsSystem{ nullptr };

	std::shared_ptr<Camera> mCamera{ nullptr };
	std::shared_ptr<TTF_Font> mFont{ nullptr };

protected:
	virtual void setup();
	virtual void handleEvents();
	virtual void update();
	virtual void draw();
	virtual void delay(Uint32 frameTime);
	virtual void teardown();
};


#endif // !__RTS__
