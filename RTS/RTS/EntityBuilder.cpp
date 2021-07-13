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
		physicsComponent->setCollider(ColliderPtr(GCC_NEW Collider((GCC_NEW AABBColliderShape(std::make_shared<Vector2f>(x, y), w, h)))));
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

	LuaScriptPtr script = this->mLuaScriptFactory->create(scriptPath, mEntity->id);

	LuaScriptSystemPtr scriptSystem = makeShared<LuaScriptSystem>(mSystemManager->getSystemByType<LuaScriptSystem>(SystemType::LUA_SCRIPT));
	scriptSystem->registerLuaScript(mEntity->id, script);

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