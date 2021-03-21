#pragma once

#include<selene.h>

class LuaScript;
typedef shared_ptr<LuaScript> LuaScriptPtr;
typedef weak_ptr<LuaScript> WeakLuaScriptPtr;
typedef unordered_map<unsigned long, LuaScriptPtr> LuaScripts;

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