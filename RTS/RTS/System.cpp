#include"System.h"

SystemManager::SystemManager(GraphicsConfig* graphicsConfig) {
	SystemManagerPtr ptr(this);
	systems.emplace(SystemType::ANIMATION, AnimationSystemPtr(GCC_NEW AnimationSystem(ptr)));
	systems.emplace(SystemType::ASSET, AssetSystemPtr(GCC_NEW AssetSystem(ptr)));
	systems.emplace(SystemType::ENTITY, EntitySystemPtr(GCC_NEW EntitySystem(ptr)));
	systems.emplace(SystemType::GRAPHICS, GraphicsSystemPtr(GCC_NEW GraphicsSystem(graphicsConfig, ptr)));
	systems.emplace(SystemType::PHYSICS, PhysicsSystemPtr(GCC_NEW PhysicsSystem(ptr)));
	systems.emplace(SystemType::INPUT, InputSystemPtr(GCC_NEW InputSystem(ptr)));
	systems.emplace(SystemType::SOUND, SoundSystemPtr(GCC_NEW SoundSystem(ptr)));
	systems.emplace(SystemType::LUA_SCRIPT, LuaScriptSystemPtr(GCC_NEW LuaScriptSystem(ptr)));
}

void AnimationSystem::update(Uint32 delta) {
	for (auto animation : mAnimations) {
		animation.second->update(delta);
	}
}

void AnimationSystem::registerAnimation(unsigned long id, AnimationHandlerPtr animationHandler) {
	if (mAnimations.find(id) != mAnimations.end()) {
		return;
	}

	mAnimations.emplace(id, animationHandler);
}

void AnimationSystem::deregisterAnimation(unsigned long id) {
	if (mAnimations.find(id) != mAnimations.end()) {
		return;
	}

	mAnimations.erase(id);
}

AnimationSetPtr AnimationSystem::createAnimationSet(const string& path) {
	std::ifstream file(path);
	std::string line;
	std::string builder;
	while (std::getline(file, line)) {
		builder.append(line);
	}
	file.close();

	rapidjson::Document doc;
	doc.Parse(builder.c_str());

	std::string name(doc["name"].GetString());
	std::string imagePath(doc["spritesheet"].GetString());
	std::string defaultAnimationName(doc["defaultAnimationName"].GetString());
	int fps(doc["fps"].GetInt());

	GraphicsSystemPtr graphicsSystem = static_pointer_cast<GraphicsSystem>(mSystemManager->systems.at(SystemType::GRAPHICS));
	graphicsSystem->addTexture(imagePath, name);

	AnimationSetPtr animationSet(GCC_NEW AnimationSet());
	animationSet->defaultAnimationName = defaultAnimationName;
	animationSet->fps = fps;
	animationSet->spritesheet = imagePath;
	animationSet->name = name;
	auto animations = doc["animations"].GetArray();
	for (int index = 0; index < animations.Size(); index++) {
		auto animation = animations[index].GetObject();

		AnimationPtr anim(GCC_NEW Animation());
		anim->name =animation["name"].GetString();
		animationSet->animations.emplace(anim->name, anim);
		auto frames = animation["frames"].GetArray();
		for (int frameIndex = 0; frameIndex < frames.Size(); frameIndex++) {
			auto frame = frames[frameIndex].GetObject();
			float x(float(frame["x"].GetInt()));
			float y(float(frame["y"].GetInt()));
			float w(float(frame["w"].GetInt()));
			float h(float(frame["h"].GetInt()));

			TexturePtr texture(GCC_NEW Texture(name, x, y, w, h));
			anim->frames.push_back(texture);
		}
	}

	return animationSet;
}

void  AnimationSystem::clear() {
	mAnimations.clear();
}

AssetPtr AssetSystem::DefaultAssetVendor::getAsset(const string& tag) {
	return mAssetSystem->getAsset(tag);
}

void AssetSystem::clear() {
	mAssets.clear();
}

void GraphicsSystem::registerDrawable(const unsigned long id, DrawablePtr drawable) {
	if (mDrawables.find(id) == mDrawables.end()) {
		mDrawables.emplace(id, vector<DrawablePtr>());
	}
	mDrawables.at(id).push_back(drawable);
	mReverseLookup.emplace(drawable, id);
	mDrawableList.push_back(drawable);
	sortDrawableList();
}

void  GraphicsSystem::deregisterDrawable(const unsigned long id) {
	if (mDrawables.find(id) != mDrawables.end()) {
		for (DrawablePtr drawable : mDrawables.at(id)) {
			mReverseLookup.erase(drawable);
			mDrawableList.erase(std::find(mDrawableList.begin(), mDrawableList.end(), drawable));
		}
		mDrawables.erase(mDrawables.find(id));
	}
}

void GraphicsSystem::sortDrawableList() {
	mDrawableList.sort([](const DrawablePtr& first, const DrawablePtr& second){
		return first->getDrawDepth() < second->getDrawDepth();
	});
}

void  GraphicsSystem::draw() {
	mGraphics->onBeforeDraw();

	for (auto drawable : mDrawableList) {
		PhysicsSystemPtr physicsSystem(mSystemManager->getSystemByType<PhysicsSystem>(SystemType::PHYSICS));
		EntitySystemPtr entitySystem(mSystemManager->getSystemByType<EntitySystem>(SystemType::ENTITY));

		BodyPtr body(physicsSystem->getBody(mReverseLookup.at(drawable)));
		Vector2f& position = Vector2f(body->getPosition());

		EntityPtr entity(entitySystem->getEntityById(body->id));
		if (entity->parent != (unsigned long) ULLONG_MAX) {
			EntityPtr parent(entitySystem->getEntityById(entity->parent));
			while (parent != nullptr) {
				BodyPtr body(physicsSystem->getBody(parent->id));
				position += Vector2f(body->getPosition());
				if (parent->parent == (unsigned long) ULLONG_MAX) {
					parent = nullptr;
				}
				else {
					parent = makeShared(entitySystem->getEntityById(parent->parent));
				}
			}
		}

		if (!drawable->isUi) {
			translateToCamera(position, mCamera);
		}
		
		drawable->draw(*mGraphics, position);
	}

	mGraphics->onAfterDraw();
}

WeakCameraPtr GraphicsSystem::getCamera() {
	return mCamera;
}

void GraphicsSystem::getDrawableById(unsigned long entityId, vector<WeakDrawablePtr>& drawables) {
	for (DrawablePtr drawable : mDrawables.at(entityId)) {
		drawables.push_back(WeakDrawablePtr(drawable));
	}
}

void GraphicsSystem::addTexture(const std::string& path, const std::string& assetTag) {
	AssetSystemPtr assetSystem(mSystemManager->getSystemByType<AssetSystem>(SystemType::ASSET));
	if (assetSystem->contains(assetTag)) {
		SDL_Log("Asset already loaded.");
		return;
	}

	assetSystem->registerAsset(mGraphics->createTexture(path, assetTag));
}


void GraphicsSystem::addFont(const string& path, const string&  assetTag, int fontsize) {
	AssetSystemPtr assetSystem(mSystemManager->getSystemByType<AssetSystem>(SystemType::ASSET));
	if (assetSystem->contains(assetTag)) {
		SDL_Log("Asset already loaded.");
		return;
	}

	assetSystem->registerAsset(mGraphics->createFontAsset(path, assetTag, fontsize));
}

void GraphicsSystem::clear() {
	mDrawables.clear();
}

void EntitySystem::addEntity(EntityPtr entity) {
	mEntityMap.emplace(entity->id, entity);
	EntityCreatedEventData* eventData = GCC_NEW EntityCreatedEventData(entity->id, SDL_GetTicks());
	EventManager::getInstance().pushEvent(eventData);
}

WeakEntityPtr EntitySystem::getEntityById(unsigned long id) {
	return mEntityMap.at(id);
}

WeakEntityPtr EntitySystem::DefaultEntityVendor::getEntityById(unsigned long entityId) {


	return WeakEntityPtr( mEntitySystem->getEntityById(entityId) );
}

void EntitySystem::getAllEntities(std::vector<EntityPtr>& entities) {
	for (auto entry : mEntityMap) {
		entities.push_back(entry.second);
	}
}

void EntitySystem::deregisterEntity(unsigned long id) {
	EntityDestroyedEventData* eventData = GCC_NEW EntityDestroyedEventData(id, SDL_GetTicks());
	EventManager::getInstance().pushEvent(eventData);
}

void EntitySystem::update(Uint32 delta) {
	for (auto entity : mEntityMap) {
		entity.second->update();
	}
}

void EntitySystem::clear() {
	mEntityMap.clear();
}

void LuaScriptSystem::registerLuaScript(unsigned long id, const LuaScriptPtr& luaScript) {
	mLuaScripts.emplace(id, luaScript);
}

void LuaScriptSystem::deregisterLuaScript(unsigned long id) {
	if (mLuaScripts.find(id) == mLuaScripts.end()) {
		return;
	}

	mLuaScripts.erase(id);
}

void LuaScriptSystem::update(Uint32 delta) {

	for (auto entry : mLuaScripts) {
		entry.second->invoke("update", (int)delta);
	}
}

void LuaScriptSystem::clear() {
	mLuaScripts.clear();
}

void PhysicsSystem::registerBody(const unsigned long id, BodyPtr body) {
	mBodies.emplace(id, body);
}

void PhysicsSystem::deregisterBody(const unsigned long id) {
	if (mBodies.find(id) != mBodies.end()) {
		quadTree->removeBody(mBodies.at(id));
		mBodies.erase(mBodies.find(id));
	}
}

void PhysicsSystem::setWorldSize(int width, int height) {
	if (this->quadTree != nullptr) {
		this->quadTree->clear();
	}

	quadTree.reset(GCC_NEW Quadtree(width / 2, height / 2, width, height));
	for (auto& it = this->mBodies.begin(); it != this->mBodies.end(); it++) {
		if (it->second->isCollidable()) {
			this->quadTree->addBody(it->second);
		}
	}
}

WeakBodyPtr PhysicsSystem::getBody(const unsigned long id) {
	return WeakBodyPtr(mBodies.at(id));
}

void PhysicsSystem::update(Uint32 delta) {
	float step(float(delta) / float(1000));

	// TODO: add steering later.
	for (auto element : mBodies) {
		for (auto behavior : mBehaviors) {
			if (!behavior->updateBehavior(step, element.second, quadTree) || !element.second->isCollidable()) {
				continue;
			}

			// update the bodies location in the tree.
			quadTree->removeBody(element.second);
			quadTree->addBody(element.second);

			vector<WeakBodyPtr> collidingBodies;
			quadTree->getCollidingBodies(element.second, collidingBodies);

			for (WeakBodyPtr weakBody : collidingBodies) {
				BodyPtr body = makeShared(weakBody);

				EntityCollisionEventData* eventData = GCC_NEW EntityCollisionEventData(element.second->id, body->id, SDL_GetTicks());
				EventManager::getInstance().pushEvent(eventData);
			}
		}
	}
}

void PhysicsSystem::clear() {
	mBodies.clear();
	quadTree->clear();
}

void InputSystem::registerEventListener(InputListenerPtr inputListener) {
	mListeners.emplace(inputListener->id, inputListener);
}

void InputSystem::deregisterEventListener(unsigned long id) {
	mListeners.erase(id);
}

void InputSystem::handleEvent(const SDL_Event& evt) {
	int x, y;
	SDL_GetMouseState(&x, &y);
	Vector2fPtr mousePosition(GCC_NEW Vector2f(x, y));
	MouseEventPtr mouseEvent(GCC_NEW MouseEvent());
	mouseEvent->button = ((evt.button.button == SDL_BUTTON_LEFT) ? MouseButton::LEFT : MouseButton::RIGHT);
	mouseEvent->position->x = x;
	mouseEvent->position->y = y;

	KeyboardEventPtr keyEvent(GCC_NEW KeyboardEvent());
	keyEvent->key = evt.key.keysym.sym;
	keyEvent->ctrlDown = (evt.key.keysym.mod & KMOD_CTRL);
	keyEvent->shiftDown = (evt.key.keysym.mod & KMOD_SHIFT);

	EventPtr inputEvent(GCC_NEW InputEvent());
	inputEvent->keyEvent = keyEvent;
	inputEvent->mouseEvent = mouseEvent;

	InputEventType type = InputEventType::NONE;
	switch (evt.type) {
	case SDL_KEYDOWN: {
			type = InputEventType::KEY_DOWN;

			KeyEventData* keyDownEventData = GCC_NEW KeyEventData(SDL_GetTicks(), keyEvent->key,KeyAction::DOWN,  keyEvent->ctrlDown, keyEvent->shiftDown);
			EventManager::getInstance().pushEvent(keyDownEventData);

			break;
		}
	case SDL_KEYUP: {
			type = InputEventType::KEY_UP;

			KeyEventData* keyUpEventData = GCC_NEW KeyEventData(SDL_GetTicks(), keyEvent->key, KeyAction::UP, keyEvent->ctrlDown, keyEvent->shiftDown);
			EventManager::getInstance().pushEvent(keyUpEventData);

			break;
		}
	case SDL_MOUSEBUTTONDOWN: {
			type = InputEventType::MOUSE_BUTTON_DOWN;

			MouseEventData* mouseDownEventData = GCC_NEW MouseEventData(SDL_GetTicks(), mouseEvent->position->x, mouseEvent->position->y, mouseEvent->button, MouseAction::CLICK_DOWN);
			EventManager::getInstance().pushEvent(mouseDownEventData);

			break;
		}
	case SDL_MOUSEBUTTONUP: {
			type = InputEventType::MOUSE_BUTTON_UP;

			MouseEventData* mouseUpEventData = GCC_NEW MouseEventData(SDL_GetTicks(), mouseEvent->position->x, mouseEvent->position->y, mouseEvent->button, MouseAction::CLICK_UP);
			EventManager::getInstance().pushEvent(mouseUpEventData);

			break;
		}
	case SDL_MOUSEMOTION: {
			type = InputEventType::MOUSE_MOVE;
			break;
		}
	}
	if (type == InputEventType::NONE) {
		return;
	}

	for (auto listener : mListeners) {
		if (listener.second->onEvent(type, inputEvent, mMouseMovementHandler)) {
			return;
		}
	}
}

void InputSystem::clear() {
	mListeners.clear();
}

bool DefaultMouseMovementHandler::checkForMouseOver(unsigned long id, const Vector2f& position) {
	GraphicsSystemPtr graphicsSystem(mSystemManager->getSystemByType<GraphicsSystem>(SystemType::GRAPHICS));
	PhysicsSystemPtr physicsSystem(mSystemManager->getSystemByType<PhysicsSystem>(SystemType::PHYSICS));
	BodyPtr body(physicsSystem->getBody(id));

	CameraPtr cam = makeShared(graphicsSystem->getCamera());
	Vector2f pos(position);
	pos += cam->position;

	return body->checkPoint(pos);
}

void DefaultSoundController::play(int loop) {
	AssetSystemPtr assetSystem(mSystemManager->getSystemByType<AssetSystem>(SystemType::ASSET));
	AssetPtr asset(assetSystem->getAsset(mSound->assetTag));
	if (SoundType::SOUND == mSound->soundType) {
		shared_ptr<Mix_Chunk> mix = makeShared(asset->getAsset<Mix_Chunk>());
		Mix_PlayChannel(-1, mix.get(), loop);
	}
	else if (SoundType::MUSIC == mSound->soundType) {
		shared_ptr<Mix_Music> mix(asset->getAsset<Mix_Music>());
		Mix_PlayMusic(mix.get(), -1);
	}
}

void DefaultSoundController::pause() {
	if (SoundType::SOUND == mSound->soundType) {
		// no op
	}
	else if (SoundType::MUSIC == mSound->soundType) {
		Mix_PauseMusic();
	}
}

void DefaultSoundController::stop() {
	if (SoundType::SOUND == mSound->soundType) {
		// no op
	}
	else if (SoundType::MUSIC == mSound->soundType) {
		Mix_HaltMusic();
	}
}

void SoundSystem::loadSound(const std::string& path, const std::string& assetTag, SoundType soundType) {
	AssetSystemPtr assetSystem = static_pointer_cast<AssetSystem>(mSystemManager->systems.at(SystemType::ASSET));
	if (assetSystem->contains(assetTag)) {
		return;
	}

	AssetPtr asset = nullptr;
	if (soundType == SoundType::SOUND) {
		VoidPtr assetPtr(Mix_LoadWAV(path.c_str()), SDL_DELETERS());
		asset = AssetPtr(GCC_NEW Asset(assetPtr, path, assetTag));
	}
	else if (soundType == SoundType::MUSIC) {
		VoidPtr assetPtr(Mix_LoadMUS(path.c_str()), SDL_DELETERS());
		asset = AssetPtr(GCC_NEW Asset(assetPtr, path, assetTag));
	}

	if (asset == nullptr) {
		std::string error("Error loading sound: ");
		error.append(SDL_GetError());
		throw std::exception(error.c_str());
	}

	assetSystem->registerAsset(asset);
}

SoundControllerPtr SoundSystem::createController(const std::string& assetTag, SoundType soundType) {
	return SoundControllerPtr(GCC_NEW DefaultSoundController(SoundPtr(GCC_NEW Sound(assetTag, soundType)), mSystemManager));
}