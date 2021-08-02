#include"System.h"

SystemManager::SystemManager(GraphicsConfig* graphicsConfig) {
	SystemManagerPtr ptr(this);
	systems.emplace(SystemType::ANIMATION, AnimationSystemPtr(GCC_NEW AnimationSystem(ptr)));
	systems.emplace(SystemType::ASSET, AssetSystemPtr(GCC_NEW AssetSystem(ptr)));
	systems.emplace(SystemType::DATA, DataSystemPtr(GCC_NEW DataSystem(ptr)));
	systems.emplace(SystemType::ENTITY, EntitySystemPtr(GCC_NEW EntitySystem(ptr)));
	systems.emplace(SystemType::GRAPHICS, GraphicsSystemPtr(GCC_NEW GraphicsSystem(graphicsConfig, ptr)));
	systems.emplace(SystemType::MAP, MapSystemPtr(GCC_NEW MapSystem(ptr)));
	systems.emplace(SystemType::PARTICLE, ParticleSystemPtr(GCC_NEW ParticleSystem(ptr)));
	systems.emplace(SystemType::PHYSICS, PhysicsSystemPtr(GCC_NEW PhysicsSystem(ptr)));
	systems.emplace(SystemType::INPUT, InputSystemPtr(GCC_NEW InputSystem(ptr)));
	systems.emplace(SystemType::SOUND, SoundSystemPtr(GCC_NEW SoundSystem(ptr)));
	systems.emplace(SystemType::LUA_SCRIPT, LuaScriptSystemPtr(GCC_NEW LuaScriptSystem(ptr)));
}

void SystemManager::update(Uint32 delta) {

	auto start = std::chrono::high_resolution_clock::now();
	LuaScriptSystemPtr luaScriptSystem = static_pointer_cast<LuaScriptSystem>(this->systems.at(SystemType::LUA_SCRIPT));
	luaScriptSystem->update(delta);

	auto end = std::chrono::high_resolution_clock::now();
	auto res = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
	DEBUG_LOG("	Script System Update: " + res.count());

	start = std::chrono::high_resolution_clock::now();
	PhysicsSystemPtr physicsSystem = static_pointer_cast<PhysicsSystem>(this->systems.at(SystemType::PHYSICS));
	physicsSystem->update(delta);

	end = std::chrono::high_resolution_clock::now();
	res = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
	DEBUG_LOG("	Physics System Setup: " + res.count());

	start = std::chrono::high_resolution_clock::now();
	AnimationSystemPtr animationSystem = static_pointer_cast<AnimationSystem>(this->systems.at(SystemType::ANIMATION));
	animationSystem->update(delta);

	end = std::chrono::high_resolution_clock::now();
	res = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
	DEBUG_LOG("	Animation System Setup: " + res.count());

	start = std::chrono::high_resolution_clock::now();
	ParticleSystemPtr particleSystem = static_pointer_cast<ParticleSystem>(this->systems.at(SystemType::PARTICLE));
	particleSystem->update(delta);

	end = std::chrono::high_resolution_clock::now();
	res = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
	DEBUG_LOG("	Particle System Setup: " + res.count());

	start = std::chrono::high_resolution_clock::now();
	EntitySystemPtr entitySystem = static_pointer_cast<EntitySystem>(this->systems.at(SystemType::ENTITY));
	entitySystem->update(delta);

	end = std::chrono::high_resolution_clock::now();
	res = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
	DEBUG_LOG("	Entity System Setup: " + res.count());

	start = std::chrono::high_resolution_clock::now();
	MapSystemPtr mapSystem = static_pointer_cast<MapSystem>(this->systems.at(SystemType::MAP));
	mapSystem->update(delta);

	end = std::chrono::high_resolution_clock::now();
	res = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
	DEBUG_LOG("	Map System Setup: " + res.count());
}

void SystemManager::clear() {
	for (auto system : this->systems) {
		system.second->clear();
	}
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

AnimationSetPtr AnimationSystem::loadAnimationSet(const string& path) {
	GraphicsSystemPtr graphicsSystem = mSystemManager->getSystemByType<GraphicsSystem>(SystemType::GRAPHICS);
	AssetSystemPtr assetSystem = mSystemManager->getSystemByType<AssetSystem>(SystemType::ASSET);
	if (assetSystem->contains(path)) {
		AssetPtr animationSetAsset = assetSystem->getAsset(path);
		return animationSetAsset->getAsset<AnimationSet>();
	}

	std::ifstream file;
	file.open(path);
	if (!file.is_open()) {
		ERR("Unable to open path: " + path);
	}
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

	graphicsSystem->addTexture(imagePath, name);

	AnimationSet* animationSet(GCC_NEW AnimationSet());
	animationSet->defaultAnimationName = defaultAnimationName;
	animationSet->fps = fps;
	animationSet->spritesheet = imagePath;
	animationSet->name = name;
	const rapidjson::Value& animations = doc["animations"];
	for (int index = 0; index < animations.Size(); index++) {
		const rapidjson::Value& animation = animations[index];

		AnimationPtr anim(GCC_NEW Animation());
		anim->name = animation["name"].GetString();
		animationSet->animations.emplace(anim->name, anim);
		const rapidjson::Value& frames = animation["frames"];
		for (int frameIndex = 0; frameIndex < frames.Size(); frameIndex++) {
			const rapidjson::Value& frame = frames[frameIndex];
			float x(float(frame["x"].GetInt()));
			float y(float(frame["y"].GetInt()));
			float w(float(frame["w"].GetInt()));
			float h(float(frame["h"].GetInt()));

			TexturePtr texture(GCC_NEW Texture(name, x, y, w, h));
			anim->frames.push_back(texture);
		}
	}

	VoidPtr voidPtr(animationSet);
	AssetPtr asset = std::make_shared<Asset>(voidPtr, path, path);
	assetSystem->registerAsset(asset);

	return AnimationSetPtr(animationSet);
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

void DataSystem::load(const string& path) {
	if (data.find(path) != data.end()) {
		throw "Data file with path " + path + " already exists.";
	}

	this->data.emplace(path, std::make_shared<SimpleDataStore>(path));
}

void DataSystem::save(const string& path) {
	this->data.at(path)->save();
}

void DataSystem::close(const string& path, bool saveFirst) {
	if (saveFirst) {
		this->save(path);
	}

	this->data.erase(data.find(path));
}

const bool DataSystem::hasData(const string& dataStore, const string& key) {
	if (this->data.find(dataStore) == this->data.end()) {
		throw "Datastore not found " + dataStore;
	}
	auto ds = this->data.at(dataStore);

	return ds->data.find(key) != ds->data.end();
}

const string& DataSystem::getData(const string& dataStore, const string& key) {
	if (this->data.find(dataStore) == this->data.end()) {
		throw "Datastore not found " + dataStore;
	}
	auto ds = this->data.at(dataStore);
	if (ds->data.find(key) == ds->data.end()) {
		return "";
	}
	return ds->getData(key);
}

void DataSystem::putData(const string& dataStore, const string& key, const string& val) {
	this->data.at(dataStore)->pushData(key, val);
}

bool DataSystem::contains(const string& dataStore) {
	return this->data.find(dataStore) != data.end();
}

void DataSystem::clear() {
	this->data.clear();
}

void GraphicsSystem::initialize() {
	EventDelegate zOrderChangeDelegate([this](const EventData& eventData) {
		sortDrawableList();
		});
	EventListenerDelegate zOrderChanged(zOrderChangeDelegate);
	EventManager::getInstance().addDelegate(zOrderChanged, EventType::ENTITY_ZORDER_SET);

	EventDelegate destroyEntityDelegate([this](const EventData& eventData) {
		EntityDestroyedEventData data = dynamic_cast<const EntityDestroyedEventData&>(eventData);

		deregisterDrawable(data.getEntityId());
		});

	EventDelegate loadAsset([this](const EventData& eventData) {
		LoadAssetEventData loadAssetEvent = dynamic_cast<const LoadAssetEventData&>(eventData);
		if (loadAssetEvent.path.find(".png") != std::string::npos) {
			this->addTexture(loadAssetEvent.path, loadAssetEvent.assetTag);
		}
		});
	EventListenerDelegate loadAssetDelegate(loadAsset);
	EventManager::getInstance().addDelegate(loadAssetDelegate, EventType::LOAD_ASSET);

	EventListenerDelegate destroyEntityListener(destroyEntityDelegate);
	EventManager::getInstance().addDelegate(destroyEntityListener, EventType::ENTITY_DESTROYED);

	this->sortDrawableList();
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
	if (sortedThisFrame) {
		return;
	}
	mDrawableList.sort([](const DrawablePtr& first, const DrawablePtr& second){
		return first->getDrawDepth() < second->getDrawDepth();
	});
	sortedThisFrame = true;
}

void  GraphicsSystem::draw() {
	mGraphics->onBeforeDraw();

	int totalCount = 0;
	int drawnCount = 0;

	for (auto drawable : mDrawableList) {
		totalCount++;
		if (!drawable->isShowing()) {
			continue;
		}

		PhysicsSystemPtr physicsSystem = mSystemManager->getSystemByType<PhysicsSystem>(SystemType::PHYSICS);
		EntitySystemPtr entitySystem = mSystemManager->getSystemByType<EntitySystem>(SystemType::ENTITY);

		BodyPtr body(physicsSystem->getBody(mReverseLookup.at(drawable)));
		Vector2f& position = Vector2f(body->getPosition());
		drawnCount++;

		EntityPtr entity(entitySystem->getEntityById(body->id));
		if (entity->parent != nullptr) {
			EntityPtr parent = entity->parent;
			while (parent != nullptr) {
				BodyPtr body(physicsSystem->getBody(parent->id));
				position += Vector2f(body->getPosition());
				if (parent->parent == nullptr) {
					parent = nullptr;
				}
				else {
					parent = parent->parent;
				}
			}
		}

		if (!drawable->isUi) {
			translateToCamera(position, mCamera);
		}

		if (!drawable->isOnScreen(position.x, position.y, mCamera->position->x + (mCamera->width / 2.0f), mCamera->position->y + (mCamera->height / 2.0f), mCamera->width, mCamera->height)) {
			continue;
		}
		
		drawable->draw(*mGraphics, position);
		/*
		if (__DEBUG__) {
			if (body->collider != nullptr) {
				Vector2f pos = *body->collider->colliderShape->position;
				mGraphics->drawBox(pos.x, pos.y, body->width, body->height, 0, 255, 0, 255);
			}
		}
		*/
	}


	// reset this flag before the next frame.
	sortedThisFrame = false;

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
	AssetSystemPtr assetSystem = mSystemManager->getSystemByType<AssetSystem>(SystemType::ASSET);
	if (assetSystem->contains(assetTag)) {
		SDL_Log("Asset already loaded.");
		return;
	}

	assetSystem->registerAsset(mGraphics->createTexture(path, assetTag));
}

void GraphicsSystem::drawTexture(const string& assetTag, float x, float y, float w, float h, float tx, float ty, float tw, float th, float angle, Uint8 r, Uint8 g, Uint8 b, Uint8 a) {
	AssetSystemPtr assetSystem = mSystemManager->getSystemByType<AssetSystem>(SystemType::ASSET);
	if (!assetSystem->contains(assetTag)) {
		ERR("No texture with assetTag: " + assetTag);
		return;
	}

	AssetPtr asset = assetSystem->getAsset(assetTag);
	shared_ptr<SDL_Texture> texture = asset->getAsset<SDL_Texture>();
	mGraphics->renderTexture(texture.get(), x, y, w, h, tx, ty, tw, th, angle, r, g, b, a);
}

void GraphicsSystem::drawTexture(TexturePtr texture, float x, float y, float w, float h, float tx, float ty, float tw, float th, float angle, Uint8 r, Uint8 g, Uint8 b, Uint8 a) {
	mGraphics->renderTexture(texture, x, y, w, h, angle, r, g, b, a);
}

void GraphicsSystem::addTexture(const string& assetTag, int width, int height) {
	AssetSystemPtr assetSystem = mSystemManager->getSystemByType<AssetSystem>(SystemType::ASSET);
	if (assetSystem->contains(assetTag)) {
		SDL_Log("Asset already loaded.");
		return;
	}

	assetSystem->registerAsset(mGraphics->createTexture(width, height, assetTag));
}

void GraphicsSystem::drawToTexture(const string& assetTag) {
	AssetSystemPtr assetSystem = mSystemManager->getSystemByType<AssetSystem>(SystemType::ASSET);
	AssetPtr textureAsset = assetSystem->getAsset(assetTag);
	shared_ptr<SDL_Texture> texture = textureAsset->getAsset<SDL_Texture>();

	mGraphics->drawToTexture(&*texture);
}

void GraphicsSystem::drawToScreen() {
	mGraphics->drawToScreen();
}

void GraphicsSystem::addFont(const string& path, const string&  assetTag, int fontsize) {
	AssetSystemPtr assetSystem = mSystemManager->getSystemByType<AssetSystem>(SystemType::ASSET);
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

EntityPtr EntitySystem::getEntityById(unsigned long id) {
	return mEntityMap.at(id);
}

EntityPtr EntitySystem::DefaultEntityVendor::getEntityById(unsigned long entityId) {


	return mEntitySystem->getEntityById(entityId);
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

void MapSystem::setMap(MapPtr map) {
	this->map = map;
}

MapPtr MapSystem::getMap() {
	return this->map;
}

void MapSystem::registerGridHandler(unsigned long entityId, GridHandlerPtr gridHandler) {
	if (this->grids.find(entityId) != this->grids.end()) {
		throw "Grid with id already registered. " + std::to_string(entityId);
	}

	this->grids[entityId] = gridHandler;
}

void MapSystem::deregisterGridHandler(unsigned long entityId) {
	if (this->grids.find(entityId) == this->grids.end()) {
		return;
	}

	this->grids.erase(this->grids.find(entityId));
}

void MapSystem::update(Uint32 delta) {
	GraphicsSystemPtr graphicsSystem = mSystemManager->getSystemByType<GraphicsSystem>(SystemType::GRAPHICS);
	GraphicsPtr graphics = graphicsSystem->getGraphics();
	for (auto gridHandler : this->grids) {
		graphicsSystem->drawToTexture(gridHandler.second->getGrid()->name);
		gridHandler.second->update(*graphics.get());
	}

	graphicsSystem->drawToScreen();
}

void MapSystem::clear() {
	this->grids.clear();
}

void ParticleSystem::registerParticleEmitter(unsigned long id, const ParticleEmitterPtr& emitter) {
	this->particleEmitters.emplace(id, emitter);
}

void ParticleSystem::deregisterParticleEmitter(unsigned long id) {
	if (this->particleEmitters.find(id) == this->particleEmitters.end()) {
		return;
	}

	this->particleEmitters.erase(this->particleEmitters.find(id));
}

void ParticleSystem::update(Uint32 delta) {
	for (auto it = this->particleEmitters.begin(); it != this->particleEmitters.end(); it++) {
		it->second->update(delta);
	}
}

void ParticleSystem::clear() {
	this->particleEmitters.clear();
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
			BodyPtr body = element.second;
			if (!behavior->updateBehavior(step, body, quadTree) || !body->isCollidable()) {
				continue;
			}

			// update the bodies location in the tree.
			quadTree->removeBody(element.second);
			quadTree->addBody(element.second);

			// Some behaviours stop the body before overlapping with other bodies. Move the body forward one pixel to catch any potential collisions.
			/*
			vector<WeakBodyPtr> collidingBodies;
			quadTree->getCollidingBodies(element.second, collidingBodies);

			for (WeakBodyPtr weakBody : collidingBodies) {
				BodyPtr collidedBody = makeShared(weakBody);

				EntityCollisionEventData* eventData = GCC_NEW EntityCollisionEventData(body->id, collidedBody->id, SDL_GetTicks());
				EventManager::getInstance().pushEvent(eventData);
			}
			*/
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

			MouseEventData* mouseMoveEventData = GCC_NEW MouseEventData(SDL_GetTicks(), mouseEvent->position->x, mouseEvent->position->y, mouseEvent->button, MouseAction::MOVE);
			EventManager::getInstance().pushEvent(mouseMoveEventData);

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

bool InputSystem::contains(unsigned long id) {
	return this->mListeners.find(id) != this->mListeners.end();
}

WeakInputListenerPtr InputSystem::at(unsigned long id) {
	return WeakInputListenerPtr(this->mListeners.at(id));
}

void InputSystem::clear() {
	mListeners.clear();
}

bool DefaultMouseMovementHandler::checkForMouseOver(unsigned long id, const Vector2f& position) {
	GraphicsSystemPtr graphicsSystem = mSystemManager->getSystemByType<GraphicsSystem>(SystemType::GRAPHICS);
	PhysicsSystemPtr physicsSystem = mSystemManager->getSystemByType<PhysicsSystem>(SystemType::PHYSICS);
	EntitySystemPtr entitySystem = mSystemManager->getSystemByType<EntitySystem>(SystemType::ENTITY);
	auto ent = entitySystem->getEntityById(id);
	BodyPtr body(physicsSystem->getBody(id));

	CameraPtr cam = makeShared(graphicsSystem->getCamera());
	Vector2f pos(position);
	pos += cam->position;

	return body->checkPoint(pos);
}

void DefaultSoundController::play(int loop) {
	AssetSystemPtr assetSystem = mSystemManager->getSystemByType<AssetSystem>(SystemType::ASSET);
	AssetPtr asset(assetSystem->getAsset(mSound->assetTag));
	if (SoundType::SOUND == mSound->soundType) {
		shared_ptr<Mix_Chunk> mix = asset->getAsset<Mix_Chunk>();
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