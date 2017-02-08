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

	// test 
	vector<EntityPtr> mBlocks;
	vector<EntityPtr> mOtherBlocks;

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


#endif // !__RTS__
