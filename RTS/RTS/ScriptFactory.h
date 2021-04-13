#pragma once

#include"EntityFactory.h"
#include"Script.h"
#include"System.h"
#include"Ui.h"

#include<cmath>

class LuaScriptFactory;
typedef shared_ptr<LuaScriptFactory> LuaScriptFactoryPtr;
typedef weak_ptr<LuaScriptFactory> WeakLuaScriptFactoryPtr;

class LuaScriptFactory {
public:
	LuaScriptFactory(
		WeakEntityFactoryPtr& entityFactory,
		WeakWidgetFactoryPtr& widgetFactory,
		WeakSystemManagerPtr& systemManager) {

		this->mEntityFactory = entityFactory;
		this->mWidgetFactory = widgetFactory;
		this->mSystemManager = systemManager;

		EventDelegate scriptLoadedDelegate([this](const EventData& eventData) {
			ScriptLoadedData data = dynamic_cast<const ScriptLoadedData&>(eventData);

			initialize(data.script);
		});

		EventListenerDelegate scriptLoadedListener(scriptLoadedDelegate);
		EventManager::getInstance().addDelegate(scriptLoadedListener, EventType::SCRIPT_LOADED);
	}

	LuaScriptPtr create(const string& scriptPath, unsigned long entityId);

private:
	void initialize(LuaScriptPtr& script);
	void registerFactory(LuaScriptPtr& script);
	void registerEntity(LuaScriptPtr& script);
	void registerPhysics(LuaScriptPtr& script);
	void registerDrawable(LuaScriptPtr& script);
	void registerAnimation(LuaScriptPtr& script);
	void registerInput(LuaScriptPtr& script);
	void registerScript(LuaScriptPtr& script);
	void registerUi(LuaScriptPtr& script);
	void registerAsset(LuaScriptPtr& script);
	void registerCamera(LuaScriptPtr& script);
	void registerMouseMove(LuaScriptPtr& script);

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

	void setX(double x) {
		this->vector->x = x;
	}

	void setY(double y) {
		this->vector->y = y;
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

	void muliply(LuaFriendlyVector2f& vec) {
		*this->vector *= *vec.vector;
	}

	void scale(double scaler) {
		this->vector->x *= scaler;
		this->vector->y *= scaler;
	}

	double dot(LuaFriendlyVector2f& vec) {
		return this->vector->dot(vec.vector);
	}

	Vector2fPtr vector;
};

class LuaFriendlyIntVector {
public:
	LuaFriendlyIntVector() {}

	int size() {
		return this->vector.size();
	}

	int at(int index) {
		return this->vector.at(index);
	}

	void push(int value) {
		return vector.push_back(value);
	}

private:
	vector<int> vector;
};

class LuaFriendlyStringVector {
public:
	LuaFriendlyStringVector() {}

	int size() {
		return this->vector.size();
	}

	string at(int index) {
		return this->vector.at(index);
	}

	void push(string value) {
		vector.push_back(value);
	}

private:
	vector<string> vector;
};

class LuaFriendlyStringMap {
public:
	LuaFriendlyStringMap() {}

	int size() {
		return this->map.size();
	}

	string at(string key) {
		return this->map.at(key);
	}

	void put(string key, string value) {
		map.emplace(key, value);
	}

private:
	unordered_map<string, string> map;
};