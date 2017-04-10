#include"EntityBuilder.h"


EntityBuilder& EntityBuilder::withPhysics(float x, float y, float w, float h, bool collidable) {

	if (mEntity->getComponents().find(ComponentType::PHYSICS_COMPONENT) != mEntity->getComponents().end()) {
		return *this;
	}

	PhysicsSystemPtr physicsSystem = makeShared(mSystemManager->getSystemByType<PhysicsSystem>(SystemType::PHYSICS));
	BodyPtr blockBody(GCC_NEW Body(mEntity->id, x, y, w, h));
	physicsSystem->registerBody(mEntity->id, blockBody);
	PhysicsComponentPtr physicsComponent(GCC_NEW PhysicsComponent(mEntity->id, blockBody));
	if (collidable) {
		physicsComponent->setCollider(GCC_NEW Collider(x, y, w, h));
	}

	mEntity->addComponent(ComponentPtr(physicsComponent));

	return *this;
}

EntityBuilder& EntityBuilder::withTexture(const string& texturePath, float tx, float ty, float w, float h) {

	if (mEntity->getComponents().find(ComponentType::DRAWABLE_COMPONENT) != mEntity->getComponents().end()) {
		return *this;
	}

	GraphicsSystemPtr graphicsSystem = makeShared(mSystemManager->getSystemByType<GraphicsSystem>(SystemType::GRAPHICS));
	graphicsSystem->addTexture(texturePath, texturePath);

	TexturePtr texture(GCC_NEW Texture(texturePath, tx, ty, w, h));
	DrawablePtr textureDrawable(GCC_NEW TextureDrawable(texture));
	graphicsSystem->registerDrawable(mEntity->id, textureDrawable);
	DrawableComponentPtr drawableComponent(GCC_NEW DrawableComponent(mEntity->id, textureDrawable));

	mEntity->addComponent(ComponentPtr(drawableComponent));

	return *this;
}

EntityBuilder& EntityBuilder::withAnimation(const string& animationSetPath) {

	if (mEntity->getComponents().find(ComponentType::ANIMATION_COMPONENT) != mEntity->getComponents().end()) {
		return *this;
	}

	if (mEntity->getComponents().find(ComponentType::DRAWABLE_COMPONENT) == mEntity->getComponents().end()) {
		GraphicsSystemPtr graphicsSystem = makeShared(mSystemManager->getSystemByType<GraphicsSystem>(SystemType::GRAPHICS));
		TexturePtr texture(GCC_NEW Texture(""));
		shared_ptr<TextureDrawable> textureDrawable(GCC_NEW TextureDrawable(texture));
		graphicsSystem->registerDrawable(mEntity->id, textureDrawable);
		DrawableComponentPtr drawableComponent(GCC_NEW DrawableComponent(mEntity->id, textureDrawable));

		mEntity->addComponent(ComponentPtr(drawableComponent));
	}

	DrawableComponentPtr drawableComponent = makeShared<DrawableComponent>(mEntity->getComponentByType<DrawableComponent>(ComponentType::DRAWABLE_COMPONENT));
	TextureDrawablePtr textureDrawable = dynamic_pointer_cast<TextureDrawable>(makeShared<Drawable>(drawableComponent->getDrawable()));

	AnimationSystemPtr animationSystem = makeShared(mSystemManager->getSystemByType<AnimationSystem>(SystemType::ANIMATION));
	AnimationSetPtr animationSet = animationSystem->createAnimationSet(animationSetPath);
	AnimationHandlerPtr animationHandler(GCC_NEW AnimationHandler(textureDrawable, animationSet, animationSet->fps));
	animationSystem->registerAnimation(mEntity->id, animationHandler);
	AnimationComponentPtr animationComponent(GCC_NEW AnimationComponent(mEntity->id, animationHandler));

	mEntity->addComponent(ComponentPtr(animationComponent));

	return *this;
}

EntityBuilder& EntityBuilder::withScript(const string& scriptPath) {

	LuaScriptPtr script = this->mLuaScriptFactory->create(scriptPath);

	LuaScriptSystemPtr scriptSystem = makeShared<LuaScriptSystem>(mSystemManager->getSystemByType<LuaScriptSystem>(SystemType::LUA_SCRIPT));
	scriptSystem->registerLuaScript(mEntity->id, script);

	script->state["SDLK_SPACE"] = (int)SDLK_SPACE;
	script->state["SDLK_UP"] = (int)SDLK_UP;
	script->state["SDLK_RIGHT"] = (int)SDLK_RIGHT;
	script->state["SDLK_DOWN"] = (int)SDLK_DOWN;
	script->state["SDLK_LEFT"] = (int)SDLK_LEFT;

	script->state["entityId"] = (int)mEntity->id;

	this->withInput(Input::ON_MOUSE_ENTER, function<bool(EventPtr)>([script](EventPtr evt) {
		script->invoke("onMouseEnter");
		return false;
	}));

	this->withInput(Input::ON_MOUSE_EXIT, function<bool(EventPtr)>([script](EventPtr evt) {
		script->invoke("onMouseExit");
		return false;
	}));

	this->withInput(Input::ON_MOUSE_DOWN, function<bool(EventPtr)>([script](EventPtr evt) {
		script->invoke("onMouseDown");
		return false;
	}));

	this->withInput(Input::ON_CLICK, function<bool(EventPtr)>([script](EventPtr evt) {
		script->invoke("onClick");
		return false;
	}));

	this->withInput(Input::ON_DRAG, function<bool(EventPtr)>([script](EventPtr evt) {
		script->invoke("onDrag");
		return false;
	}));

	this->withInput(Input::ON_KEY_DOWN, function<bool(EventPtr)>([script](EventPtr evt) {
		script->invoke("onKeyDown", evt->keyEvent->key, evt->keyEvent->ctrlDown, evt->keyEvent->shiftDown);
		return false;
	}));

	this->withInput(Input::ON_KEY_UP, function<bool(EventPtr)>([script](EventPtr evt) {
		script->invoke("onKeyUp", evt->keyEvent->key, evt->keyEvent->ctrlDown, evt->keyEvent->shiftDown);
		return false;
	}));

	LuaScriptComponentPtr scriptComponent(GCC_NEW LuaScriptComponent(mEntity->id, script));
	mEntity->addComponent(ComponentPtr(scriptComponent));

	return *this;
}

EntityBuilder& EntityBuilder::withInput(Input input, function<bool(EventPtr)>& callback) {

	if (mEntity->getComponents().find(ComponentType::INPUT_COMPONENT) == mEntity->getComponents().end()) {
		InputSystemPtr inputSystem(makeShared(mSystemManager->getSystemByType<InputSystem>(SystemType::INPUT)));
		InputListenerPtr inputListener(GCC_NEW InputListener(mEntity->id));
		inputSystem->registerEventListener(inputListener);
		InputComponentPtr inputComponent(GCC_NEW InputComponent(mEntity->id, inputListener));

		mEntity->addComponent(ComponentPtr(inputComponent));
	}

	InputComponentPtr inputComponent = makeShared<InputComponent>(mEntity->getComponentByType<InputComponent>(ComponentType::INPUT_COMPONENT));
	inputComponent->setInputCallback(input, callback);

	return *this;
}

EntityPtr& EntityBuilder::build() {
	return mEntity;
}