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

	LuaScriptPtr create(const string& scriptPath) {
		LuaScriptPtr script(GCC_NEW LuaScript(scriptPath));

		script->state["SDLK_SPACE"] = (int)SDLK_SPACE;

		script->state["SDLK_UP"] = (int)SDLK_UP;
		script->state["SDLK_RIGHT"] = (int)SDLK_RIGHT;
		script->state["SDLK_DOWN"] = (int)SDLK_DOWN;
		script->state["SDLK_LEFT"] = (int)SDLK_LEFT;

		// register helper methods
		script->state["createDefault"] = [this](int x, int y, int w, int h, int r, int g, int b, int a) {
			EntityFactoryPtr entityFactory = makeShared<EntityFactory>(mEntityFactory);
			EntityPtr entity = entityFactory->createDefault(x, y, w, h, r, g, b, a);
			return (int) entity->id;
		};

		script->state["createTextured"] = [this](string tag, int x, int y, int width, int height, int tx, int ty, int w, int h, bool collidable) {
			EntityFactoryPtr entityFactory = makeShared<EntityFactory>(mEntityFactory);
			SystemManagerPtr systemManager = makeShared<SystemManager>(mSystemManager);
			GraphicsSystemPtr graphicsSystem = makeShared<GraphicsSystem>(systemManager->getSystemByType<GraphicsSystem>(SystemType::GRAPHICS));
			graphicsSystem->addTexture(tag, tag);

			EntityPtr entity = entityFactory->createTexturedEntity(tag, x, y, width, height, tx, ty, w, h, collidable);
			return (int) entity->id;
		};

		script->state["setVelocity"] = [this](int entityId, double x, double y) {
			SystemManagerPtr systemManager = makeShared<SystemManager>(mSystemManager);
			
			EntitySystemPtr entitySystem = makeShared<EntitySystem>(systemManager->getSystemByType<EntitySystem>(SystemType::ENTITY));

			EntityPtr entity = makeShared<Entity>(entitySystem->getEntityById(entityId));

			PhysicsComponentPtr physicsComponent = makeShared<PhysicsComponent>(entity->getComponentByType<PhysicsComponent>(ComponentType::PHYSICS_COMPONENT));
			physicsComponent->setVelocity(Vector2f(x, y));
		};

		script->state["addVelocity"] = [this](int entityId, double x, double y) {
			SystemManagerPtr systemManager = makeShared<SystemManager>(mSystemManager);

			EntitySystemPtr entitySystem = makeShared<EntitySystem>(systemManager->getSystemByType<EntitySystem>(SystemType::ENTITY));

			EntityPtr entity = makeShared<Entity>(entitySystem->getEntityById(entityId));

			PhysicsComponentPtr physicsComponent = makeShared<PhysicsComponent>(entity->getComponentByType<PhysicsComponent>(ComponentType::PHYSICS_COMPONENT));
			physicsComponent->setVelocity(physicsComponent->getVelocity() + Vector2f(x, y));
		};
		
		script->state["setSpeed"] = [this](int entityId, double speed) {
			SystemManagerPtr systemManager = makeShared<SystemManager>(mSystemManager);

			EntitySystemPtr entitySystem = makeShared<EntitySystem>(systemManager->getSystemByType<EntitySystem>(SystemType::ENTITY));

			EntityPtr entity = makeShared<Entity>(entitySystem->getEntityById(entityId));

			PhysicsComponentPtr physicsComponent = makeShared<PhysicsComponent>(entity->getComponentByType<PhysicsComponent>(ComponentType::PHYSICS_COMPONENT));
			physicsComponent->setSpeed(speed);
		};

		script->state["getXPosition"] = [this](int entityId) {
			SystemManagerPtr systemManager = makeShared<SystemManager>(mSystemManager);

			EntitySystemPtr entitySystem = makeShared<EntitySystem>(systemManager->getSystemByType<EntitySystem>(SystemType::ENTITY));

			EntityPtr entity = makeShared<Entity>(entitySystem->getEntityById(entityId));

			PhysicsComponentPtr physicsComponent = makeShared<PhysicsComponent>(entity->getComponentByType<PhysicsComponent>(ComponentType::PHYSICS_COMPONENT));
			return (int) physicsComponent->getPosition().x;
		};

		script->state["destroyEntity"] = [this](int entityId) {
			SystemManagerPtr systemManager = makeShared<SystemManager>(mSystemManager);

			EntitySystemPtr entitySystem = makeShared<EntitySystem>(systemManager->getSystemByType<EntitySystem>(SystemType::ENTITY));
			entitySystem->deregisterEntity(entityId);
		};

		EntityFactoryPtr entityFactory = makeShared<EntityFactory>(mEntityFactory);
		EntityPtr physicsEntity = entityFactory->createPhysicsEntity(0, 0, 1, 1);

		InputListenerPtr inputListener(GCC_NEW InputListener(physicsEntity->id));
		inputListener->eventCallbacks[Input::ON_KEY_DOWN] = [script](EventPtr evt) {
			script->invoke("onKeyDown", (int)evt->keyEvent->key);
			return false;
		};
		inputListener->eventCallbacks[Input::ON_KEY_UP] = [script](EventPtr evt) {
			script->invoke("onKeyUp", (int)evt->keyEvent->key);
			return false;
		};

		SystemManagerPtr systemManager = makeShared<SystemManager>(mSystemManager);
		InputSystemPtr inputSystem = makeShared<InputSystem>(systemManager->getSystemByType<InputSystem>(SystemType::INPUT));
		inputSystem->registerEventListener(inputListener);

		EventDelegate destroyEntityDelegate([script](const EventData& eventData) {
			EntityCollisionEventData data = dynamic_cast<const EntityCollisionEventData&>(eventData);

			script->invoke("onCollision", (int) data.getCollidedEntityId(), (int) data.getColliderEntityId());
		});

		EventListenerDelegate destroyEntityListener(destroyEntityDelegate);
		EventManager::getInstance().addDelegate(destroyEntityListener, EventType::ENTITY_COLLISION_EVENT);

		return script;
	}

private:
	WeakEntityFactoryPtr mEntityFactory;
	WeakWidgetFactoryPtr mWidgetFactory;
	WeakSystemManagerPtr mSystemManager;
};