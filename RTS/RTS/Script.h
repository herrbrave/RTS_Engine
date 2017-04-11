#pragma once

#include<selene.h>

#include"Component.h"

class LuaScript;
typedef shared_ptr<LuaScript> LuaScriptPtr;
typedef weak_ptr<LuaScript> WeakLuaScriptPtr;
typedef unordered_map<unsigned long, LuaScriptPtr> LuaScripts;

class LuaScriptComponent;
typedef shared_ptr<LuaScriptComponent> LuaScriptComponentPtr;
typedef weak_ptr<LuaScriptComponent> WeakLuaScriptComponentPtr;

class LuaScript {
public:
	LuaScript(const string& scriptPath) {
		this->state.Load(scriptPath);
		this->path = scriptPath;
	}

	~LuaScript() {
		for (auto deleter : deleters) {
			deleter();
		}
	}

	template<typename... Args>
	const sel::Selector invoke(const string& funcName, Args&&... args) const {
		return this->state[funcName.c_str()](std::forward<Args>(args)...);
	}

	string path;
	sel::State state{ true };

	vector<function<void()>> deleters;
};

class LuaScriptComponent : public Component {
public:
	LuaScriptComponent(unsigned long entityId, const LuaScriptPtr& script) : Component(entityId, ComponentType::LUA_SCRIPT_COMPONENT) {
		this->mScript = script;
	}

	void serialize(Serializer& serializer) const override {
		serializer.writer.StartObject();

		serializer.writer.String("componentId");
		serializer.writer.Uint((Uint8)componentId);

		serializer.writer.String("script");
		serializer.writer.String(this->mScript->path.c_str());

		serializer.writer.EndObject();
	}

private:
	LuaScriptPtr mScript;
};