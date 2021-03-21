#pragma once

#include"Component.h"
#include"Event.h"

class LuaScriptComponent;
typedef shared_ptr<LuaScriptComponent> LuaScriptComponentPtr;
typedef weak_ptr<LuaScriptComponent> WeakLuaScriptComponentPtr;

class LuaScriptComponent : public Component {
public:
	LuaScriptPtr script;

	LuaScriptComponent(unsigned long entityId, const LuaScriptPtr& script) : Component(entityId, ComponentType::LUA_SCRIPT_COMPONENT) {
		this->script = script;
	}

	LuaScriptComponent(unsigned long entityId, const rapidjson::Value& root) : Component(entityId, ComponentType::LUA_SCRIPT_COMPONENT) {
		this->script = LuaScriptPtr(GCC_NEW LuaScript(root["script"].GetString()));

		this->script->state["entityId"] = (int)entityId;
		ScriptLoadedData* scriptLoaded = GCC_NEW ScriptLoadedData(SDL_GetTicks(), entityId, this->script);
		EventManager::getInstance().pushEvent(scriptLoaded);
	}

	void serialize(Serializer& serializer) const override {
		serializer.writer.StartObject();

		serializer.writer.String("componentId");
		serializer.writer.Uint((Uint8)componentId);

		serializer.writer.String("script");
		serializer.writer.String(this->script->path.c_str());

		serializer.writer.EndObject();
	}
};