#pragma once

#include<selene.h>

#include"EntityFactory.h"
#include"System.h"
#include"Ui.h"

class LuaScript;
typedef shared_ptr<LuaScript> LuaScriptPtr;
typedef weak_ptr<LuaScript> WeakLuaScriptPtr;

class LuaScriptFactory;
typedef shared_ptr<LuaScriptFactory> LuaScriptFactoryPtr;
typedef weak_ptr<LuaScriptFactory> WeakLuaScriptFactoryPtr;

class LuaScript {
public:
	LuaScript(const string& scriptPath) {
		this->state.Load(scriptPath);
	}

	template<typename... Args>
	const sel::Selector invoke(const string& funcName, Args&&... args) const {
		return this->state[funcName.c_str()](std::forward<Args>(args)...);
	}

	sel::State state;
};

class LuaScriptFactory {
public:
	LuaScriptFactory(
		WeakEntityFactoryPtr& entityFactory, 
		WeakWidgetFactoryPtr& widgetFactory, 
		WeakSystemManagerPtr& systemManager) {

		this->mEntityFactory = entityFactory;
		this->mWidgetFactory = widgetFactory;
		this->mSystemManager = systemManager;
	}

	LuaScriptPtr create(const string& scriptPath) {
		LuaScriptPtr script(GCC_NEW LuaScript(scriptPath));

		// register helper methods
		script->state["createDefault"] = [this](int x, int y, int w, int h, int r, int g, int b, int a) {
			EntityFactoryPtr entityFactory = makeShared<EntityFactory>(mEntityFactory);
			entityFactory->createDefault(x, y, w, h, r, g, b, a);
		};

		script->state["createTextured"] = [this](string tag, int x, int y, int width, int height, int tx, int ty, int w, int h, bool collidable) {
			EntityFactoryPtr entityFactory = makeShared<EntityFactory>(mEntityFactory);
			SystemManagerPtr systemManager = makeShared<SystemManager>(mSystemManager);
			GraphicsSystemPtr graphicsSystem = makeShared<GraphicsSystem>(systemManager->getSystemByType<GraphicsSystem>(SystemType::GRAPHICS));
			graphicsSystem->addTexture(tag, tag);

			entityFactory->createTexturedEntity(tag, x, y, width, height, tx, ty, w, h, collidable);
		};

		return script;
	}

private:
	WeakEntityFactoryPtr mEntityFactory;
	WeakWidgetFactoryPtr mWidgetFactory;
	WeakSystemManagerPtr mSystemManager;
};