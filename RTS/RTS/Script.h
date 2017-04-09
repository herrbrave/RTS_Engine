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

	sel::State state{ true };
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

	LuaScriptPtr create(const string& scriptPath);

private:
	void registerFactory(LuaScriptPtr& script);
	void registerEntity(LuaScriptPtr& script);
	void registerPhysics(LuaScriptPtr& script);
	void registerDrawable(LuaScriptPtr& script);

	EventListenerDelegate destroyEntityListener{ nullptr };

	WeakEntityFactoryPtr mEntityFactory;
	WeakWidgetFactoryPtr mWidgetFactory;
	WeakSystemManagerPtr mSystemManager;
};

// This is required to expose the Vector2f to Lua
class LuaFriendlyVector2f {
public:
	LuaFriendlyVector2f(double x, double y) {
		this->vector.reset(GCC_NEW Vector2f(x, y));
	}

	LuaFriendlyVector2f(const Vector2f& vector2f) {
		this->vector.reset(GCC_NEW Vector2f(vector2f.x, vector2f.y));
	}

	double getX() {
		return this->vector->x;
	}

	double getY() {
		return this->vector->y;
	}

	void normalize() {
		this->vector->normalize();
	}

	double magnitude() {
		return this->vector->magnitude();
	}

	void truncate(double max) {
		this->vector->truncate(max);
	}

	void add(LuaFriendlyVector2f& vec) {
		this->vector->x += vec.getX();
		this->vector->y += vec.getY();
	}

	void subtract(LuaFriendlyVector2f& vec) {
		this->vector->x -= vec.getX();
		this->vector->y -= vec.getY();
	}
	
	void scale(double scaler) {
		this->vector->x *= scaler;
		this->vector->y *= scaler;
	}

	Vector2fPtr vector;
};