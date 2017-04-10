#pragma once

#include"Entity.h"
#include"ScriptFactory.h"
#include"System.h"

class EntityBuilder;
typedef shared_ptr<EntityBuilder> EntityBuilderPtr;
typedef weak_ptr<EntityBuilder> WeakEntityBuilderPtr;

class EntityBuilder {
public:
	EntityBuilder(const SystemManagerPtr& systemManager, const LuaScriptFactoryPtr& luaScriptFactory) {
		this->mSystemManager = systemManager;
		this->mLuaScriptFactory = luaScriptFactory;

		EntitySystemPtr entitySystem = makeShared(mSystemManager->getSystemByType<EntitySystem>(SystemType::ENTITY));
		mEntity.reset(GCC_NEW Entity());
		entitySystem->addEntity(mEntity);
	}

	EntityBuilder& withPhysics(float x, float y, float w, float h, bool collidable);
	EntityBuilder& withTexture(const string& texturePath, float tx, float ty, float w, float h);
	EntityBuilder& withAnimation(const string& animationSetPath);
	EntityBuilder& withScript(const string& scriptPath);
	EntityBuilder& withInput(Input input, function<bool(EventPtr)>& callback);

	EntityPtr& build();

private: 
	SystemManagerPtr mSystemManager;
	LuaScriptFactoryPtr mLuaScriptFactory;
	EntityPtr mEntity{ nullptr };
};