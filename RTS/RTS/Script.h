#pragma once

#include<selene.h>

class LuaScript;
typedef shared_ptr<LuaScript> LuaScriptPtr;
typedef weak_ptr<LuaScript> WeakLuaScriptPtr;
typedef unordered_map<unsigned long, LuaScriptPtr> LuaScripts;

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