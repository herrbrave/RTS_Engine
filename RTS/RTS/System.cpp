#include"System.h"

SystemManager::SystemManager(GraphicsConfig* graphicsConfig) {
	SystemManagerPtr ptr(this);
	systems.emplace(SystemType::ANIMATION, AnimationSystemPtr(GCC_NEW AnimationSystem(ptr)));
	systems.emplace(SystemType::ASSET, AssetSystemPtr(GCC_NEW AssetSystem(ptr)));
	systems.emplace(SystemType::ENTITY, EntitySystemPtr(GCC_NEW EntitySystem(ptr)));
	systems.emplace(SystemType::PHYSICS, PhysicsSystemPtr(GCC_NEW PhysicsSystem(ptr)));
	systems.emplace(SystemType::GRAPHICS, GraphicsSystemPtr(GCC_NEW GraphicsSystem(GraphicsConfigPtr(graphicsConfig), ptr)));
	systems.emplace(SystemType::INPUT, InputSystemPtr(GCC_NEW InputSystem(ptr)));
	systems.emplace(SystemType::SOUND, SoundSystemPtr(GCC_NEW SoundSystem(ptr)));
}

void AnimationSystem::update(Uint32 delta) {
	for (auto animation : mAnimations) {
		animation.second->update(delta);
	}
}

void AnimationSystem::registerAnimation(unsigned long id, AnimationHandlerPtr animationHandler) {
	if (mAnimations.find(id) == mAnimations.end()) {
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
	mDrawables.emplace(id, drawable);
	mReverseLookup.emplace(drawable, id);
	mDrawableList.push_back(drawable);
	sortDrawableList();
}

void  GraphicsSystem::deregisterDrawable(const unsigned long id) {
	mDrawableList.erase(std::find(mDrawableList.begin(), mDrawableList.end(), mDrawables.at(id)));
	mReverseLookup.erase(mDrawables.at(id));
	mDrawables.erase(mDrawables.find(id));
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
		BodyPtr body(physicsSystem->getBody(mReverseLookup.at(drawable)));
		Vector2f& position = Vector2f(body->getPosition());
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

WeakDrawablePtr GraphicsSystem::getDrawableById(unsigned long entityId) {
	return WeakDrawablePtr(mDrawables.at(entityId));
}

void GraphicsSystem::addTexture(const std::string& path, const std::string& assetTag) {
	AssetSystemPtr assetSystem(mSystemManager->getSystemByType<AssetSystem>(SystemType::ASSET));
	if (assetSystem->contains(assetTag)) {
		SDL_Log("Asset already loaded.");
		return;
	}

	assetSystem->registerAsset(mGraphics->createTexture(path, assetTag));
}

void GraphicsSystem::createTextSurface(const std::string& text, const std::string& assetTag, Uint8 r, Uint8 g, Uint8 b, Uint8 a) {
	AssetSystemPtr assetSystem(mSystemManager->getSystemByType<AssetSystem>(SystemType::ASSET));
	if (assetSystem->contains(assetTag)) {
		SDL_Log("Text already added.");
		return;
	}

	assetSystem->registerAsset(mGraphics->createTextAsset(text, assetTag, r, g, b, a));
}

void GraphicsSystem::clear() {
	mDrawables.clear();
}

void EntitySystem::addEntity(EntityPtr entity) {
	mEntityMap.emplace(entity->id, entity);
	EntityCreatedEventData eventData(entity->id, SDL_GetTicks());
	EventManager::getInstance().pushEvent(&eventData);
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
	mEntityMap.erase(mEntityMap.find(id));
	EntityDestroyedEventData eventData(id, SDL_GetTicks());
	EventManager::getInstance().pushEvent(&eventData);
}

void EntitySystem::clear() {
	mEntityMap.clear();
}

void PhysicsSystem::registerBody(const unsigned long id, BodyPtr body) {
	mBodies.emplace(id, body);
}

void PhysicsSystem::deregisterBody(const unsigned long id) {
	quadTree->removeBody(mBodies.at(id));
	mBodies.erase(mBodies.find(id));
}


WeakBodyPtr PhysicsSystem::getBody(const unsigned long id) {
	return WeakBodyPtr(mBodies.at(id));
}

void PhysicsSystem::update(Uint32 delta) {
	float step(float(delta) / float(1000));


	// TODO: add steering later.
	for (auto element : mBodies) {
		Vector2f& velocity = Vector2f(element.second->getVelocity());
		Vector2f velocityCopy;
		velocityCopy.set(velocity);

		if (velocityCopy.x == 0 && velocityCopy.y == 0) {
			continue;
		}

		velocityCopy *= element.second->getSpeed();
		velocityCopy *= step;

		Vector2f& positionCopy = Vector2f(element.second->getPosition());
		Vector2f& newPosition = (positionCopy + velocityCopy);
		
		if (!element.second->isCollidable()) {
			element.second->setPosition(newPosition);
			quadTree->removeBody(element.second);
			quadTree->addBody(element.second);
			continue;
		}
		
		vector<BodyPtr> collidingBodies;
		quadTree->getNeigboringBodies(element.second, collidingBodies);

		if (collidingBodies.size() == 0) {
			element.second->setPosition(newPosition);
			quadTree->removeBody(element.second);
			quadTree->addBody(element.second);
			continue;
		}
		
		// Clean this up and move it into another method.
		BodyPtr copyBody = element.second;
		copyBody->setPosition(newPosition);
		for (WeakBodyPtr collidingBodyPtr : collidingBodies) {
			BodyPtr collidingBody = makeShared(collidingBodyPtr);
			if (!copyBody->collider->checkCollision(*collidingBody->collider)) {
				continue;
			}

			Vector2f normal(0, 0);
			float collisionTime = sweptAABB(*element.second, *collidingBody, velocityCopy, normal);
			if (collisionTime == 1.0f) {
				continue;
			}

			Vector2f scaledVelocity = (velocityCopy * collisionTime);
			newPosition.set(positionCopy + scaledVelocity);
			copyBody->setPosition(newPosition);

			collidingBody->collider->onCollision(*element.second->collider);
			element.second->collider->onCollision(*collidingBody->collider);
		}


		element.second->setPosition(newPosition);

		// update the bodies location in the tree.
		quadTree->removeBody(element.second);
		quadTree->addBody(element.second);
	}
}

void PhysicsSystem::clear() {
	mBodies.clear();
	quadTree->clear();
}

float PhysicsSystem::sweptAABB(Body& incidentBody, Body& otherBody, const Vector2f& velocity, Vector2f& normal) {
	ExtentPtr incidentExtent = incidentBody.collider->getExtent();
	ExtentPtr otherExtent = otherBody.collider->getExtent();

	Vector2f inverseEntry;
	Vector2f inverseExit;

	if (velocity.x > 0.0f) {
		inverseEntry.x = (otherExtent->x0 - incidentExtent->x1);
		inverseExit.x = (otherExtent->x1 - incidentExtent->x0);
	}
	else {
		inverseEntry.x = (otherExtent->x1 - incidentExtent->x0);
		inverseExit.x = (otherExtent->x0 - incidentExtent->x1);
	}

	if (velocity.y > 0.0f) {
		inverseEntry.y = (otherExtent->y0 - incidentExtent->y1);
		inverseExit.y = (otherExtent->y1 - incidentExtent->y0);
	}
	else {
		inverseEntry.y = (otherExtent->y1 - incidentExtent->y0);
		inverseExit.y = (otherExtent->y0 - incidentExtent->y1);
	}

	Vector2f entry;
	Vector2f exit;

	if (velocity.x == 0.0f) {
		entry.x = -std::numeric_limits<float>::infinity();
		exit.x = std::numeric_limits<float>::infinity();
	}
	else {
		entry.x = (inverseEntry.x / velocity.x);
		exit.x = (inverseExit.x / velocity.x);
	}

	if (velocity.y == 0.0f) {
		entry.y = -std::numeric_limits<float>::infinity();
		exit.y = std::numeric_limits<float>::infinity();
	}
	else {
		entry.y = (inverseEntry.y / velocity.y);
		exit.y = (inverseExit.y / velocity.y);
	}

	float entryTime = std::max(entry.x, entry.y);
	float exitTime = std::min(exit.x, exit.y);
	if (entryTime > exitTime || entry.x < 0.0f && entry.y < 0.0f || entry.x > 1.0f || entry.y > 1.0f) {
		normal.set(0.0f, 0.0f);
		return 1.0f;
	}

	if (entry.x > entry.y) {
		if (inverseEntry.x < 0.0f) {
			normal.set(1.0f, 0.0f);
		}
		else {
			normal.set(-1.0f, 0.0f);
		}
	}
	else {
		if (inverseEntry.y < 0.0f) {
			normal.set(0.0f, 1.0f);
		}
		else {
			normal.set(0.0f, -1.0f);
		}
	}

	return entryTime;
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
	case SDL_KEYDOWN:
		type = InputEventType::KEY_DOWN;
		break;
	case SDL_KEYUP:
		type = InputEventType::KEY_UP;
		break;
	case SDL_MOUSEBUTTONDOWN:
		type = InputEventType::MOUSE_BUTTON_DOWN;
		break;
	case SDL_MOUSEBUTTONUP:
		type = InputEventType::MOUSE_BUTTON_UP;
		break;
	case SDL_MOUSEMOTION:
		type = InputEventType::MOUSE_MOVE;
		break;

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
	PhysicsSystemPtr physicsSystem(mSystemManager->getSystemByType<PhysicsSystem>(SystemType::PHYSICS));
	BodyPtr body(physicsSystem->getBody(id));
	return body->checkPoint(position);
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