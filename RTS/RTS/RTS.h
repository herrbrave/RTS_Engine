#ifndef __RTS__
#define __RTS__

#include<memory>
#include<vector>

#include"EntityFactory.h"
#include"Flowfield.h"
#include"Game.h"
#include"MapFactory.h"
#include"System.h"
#include"Ui.h"

class RTS : public Game {
private:
	std::unique_ptr<SystemManager> mSystemManager;

	std::unique_ptr<EntityFactory> mEntityFactory{ nullptr };

	std::unique_ptr<TileFactory> mTileFactory{ nullptr };
	std::unique_ptr<MapFactory> mMapFactory{ nullptr };
	std::unique_ptr<Map> mMap{ nullptr };

	std::unique_ptr<WidgetFactory> mWidgetFactory{ nullptr };

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
