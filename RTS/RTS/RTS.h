#ifndef __RTS__
#define __RTS__

#include<stdlib.h>
#include<time.h>

#include<chrono>

#include<iostream>
#include<fstream>
#include<memory>
#include<vector>

#include"Animation.h"
#include"Flowfield.h"
#include"Game.h"
#include"MapFactory.h"
#include"Particle.h"
#include"ScriptFactory.h"
#include"SoundControllerFactory.h"
#include"System.h"
#include"WidgetFactory.h"
#include"WorldFactory.h"

class RTS : public Game {
private:
	SystemManagerPtr mSystemManager;

	EntityFactoryPtr mEntityFactory{ nullptr };
	EntityPtr mEntity{ nullptr };
	MapPtr mMap{ nullptr };
	WorldFactoryPtr worldFactory{ nullptr };
	WorldPtr world{ nullptr };

	WidgetFactoryPtr mWidgetFactory{ nullptr };

	SoundControllerFactoryPtr mSoundControllerFactory{ nullptr };

	Uint32 mLastTime{ 0 };

	LuaScriptFactoryPtr mLuaScriptFactory{ nullptr };

	EntityPtr entity{ nullptr };

	MapPtr map{ nullptr };

protected:
	void setup() override;
	void handleEvents() override;
	void update() override;
	void draw() override;
	void delay(Uint32 frameTime) override;
	void teardown() override;
};


#endif // !__RTS__
