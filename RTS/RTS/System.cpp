#include"System.h"

SystemManager::SystemManager(GraphicsConfig* graphicsConfig) {
	SystemManagerPtr ptr(this);
	systems.emplace(SystemType::ANIMATION, AnimationSystemPtr(GCC_NEW AnimationSystem(ptr)));
	systems.emplace(SystemType::ASSET, AssetSystemPtr(GCC_NEW AssetSystem(ptr)));
	systems.emplace(SystemType::DATA, DataSystemPtr(GCC_NEW DataSystem(ptr)));
	systems.emplace(SystemType::ENTITY, EntitySystemPtr(GCC_NEW EntitySystem(ptr)));
	systems.emplace(SystemType::GRAPHICS, GraphicsSystemPtr(GCC_NEW GraphicsSystem(graphicsConfig, ptr)));
	systems.emplace(SystemType::WORLD, WorldSystemPtr(GCC_NEW WorldSystem(ptr)));
	systems.emplace(SystemType::PARTICLE, ParticleSystemPtr(GCC_NEW ParticleSystem(ptr)));
	systems.emplace(SystemType::PHYSICS, PhysicsSystemPtr(GCC_NEW PhysicsSystem(ptr)));
	systems.emplace(SystemType::INPUT, InputSystemPtr(GCC_NEW InputSystem(ptr)));
	systems.emplace(SystemType::SOUND, SoundSystemPtr(GCC_NEW SoundSystem(ptr)));
	systems.emplace(SystemType::LUA_SCRIPT, LuaScriptSystemPtr(GCC_NEW LuaScriptSystem(ptr)));
}

void SystemManager::update(Uint32 delta) {

	LuaScriptSystemPtr luaScriptSystem = static_pointer_cast<LuaScriptSystem>(this->systems.at(SystemType::LUA_SCRIPT));
	luaScriptSystem->update(delta);

	PhysicsSystemPtr physicsSystem = static_pointer_cast<PhysicsSystem>(this->systems.at(SystemType::PHYSICS));
	physicsSystem->update(delta);

	AnimationSystemPtr animationSystem = static_pointer_cast<AnimationSystem>(this->systems.at(SystemType::ANIMATION));
	animationSystem->update(delta);

	ParticleSystemPtr particleSystem = static_pointer_cast<ParticleSystem>(this->systems.at(SystemType::PARTICLE));
	particleSystem->update(delta);

	EntitySystemPtr entitySystem = static_pointer_cast<EntitySystem>(this->systems.at(SystemType::ENTITY));
	entitySystem->update(delta);
}

void SystemManager::clear() {
	for (auto system : this->systems) {
		system.second->clear();
	}
}

void AnimationSystem::update(Uint32 delta) {
	for (auto animationList : mAnimations) {
		for (auto animation : animationList.second) {
			animation->update(delta);
		}
	}
}

void AnimationSystem::registerAnimation(unsigned long id, AnimationHandlerPtr animationHandler) {
	if (mAnimations.find(id) == mAnimations.end()) {
		mAnimations[id] = vector<AnimationHandlerPtr>();
	}

	mAnimations[id].push_back(animationHandler);
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
		sortDrawables = true;
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

	sortDrawables = true;
}

void GraphicsSystem::registerDrawable(const unsigned long id, DrawablePtr drawable) {
	if (mDrawables.find(id) == mDrawables.end()) {
		mDrawables.emplace(id, vector<DrawablePtr>());
	}
	mDrawables.at(id).push_back(drawable);
	mReverseLookup.emplace(drawable, id);
	mDrawableList.push_back(drawable);
	sortDrawables = true;
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

void GraphicsSystem::sortDrawableList(bool forceSort) {
	mDrawableList.sort([](const DrawablePtr& first, const DrawablePtr& second){
		return first->getDrawDepth() < second->getDrawDepth();
	});
	sortDrawables = false;
}

void  GraphicsSystem::draw() {
	mGraphics->onBeforeDraw();

	if (sortDrawables) {
		this->sortDrawableList();
	}

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

		if (!drawable->isOnScreen(position.x, position.y, mCamera->width, mCamera->height)) {
			continue;
		}
		
		drawable->draw(*mGraphics, position);
	}

	LuaScriptSystemPtr scriptSystem = mSystemManager->getSystemByType<LuaScriptSystem>(SystemType::LUA_SCRIPT);
	for (auto entry : scriptSystem->getLuaScripts()) {
		if (entry.second->state["onDraw"].exists()) {
			entry.second->invoke("onDraw");
		}
	}

	if (__DEBUG__) {
		PhysicsSystemPtr physicsSystem = mSystemManager->getSystemByType<PhysicsSystem>(SystemType::PHYSICS);
		for (auto bod : physicsSystem->mBodies) {
			SDL_Rect dest{ bod.second->position->x - mCamera->position->x - bod.second->width / 2, bod.second->position->y - mCamera->position->y - bod.second->height / 2, bod.second->width, bod.second->height };
			mGraphics->drawLine(dest.x, dest.y, dest.x + dest.w, dest.y, 0, 255, 0, 255);
			mGraphics->drawLine(dest.x + dest.w, dest.y, dest.x + dest.w, dest.y + dest.h, 0, 255, 0, 255);
			mGraphics->drawLine(dest.x + dest.w, dest.y + dest.h, dest.x, dest.y + dest.h, 0, 255, 0, 255);
			mGraphics->drawLine(dest.x, dest.y + dest.h, dest.x, dest.y, 0, 255, 0, 255);
		}
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
	mGraphics->renderTexture(texture.get(), x, y, w, h, tx, ty, tw, th, angle, r, g, b, a, SDL_FLIP_NONE);
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

void GraphicsSystem::clearTexture(const string& assetTag) {
	mGraphics->drawToTexture(assetTag);
	mGraphics->onBeforeDraw();
	mGraphics->drawToScreen();
}

void GraphicsSystem::drawToTexture(const string& assetTag) {

	mGraphics->drawToTexture(assetTag);
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

LuaScripts LuaScriptSystem::getLuaScripts() {
	return this->mLuaScripts;
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
	mBodies[id] = body;
	if (body->isCollidable()) {
		quadTree->addBody(body);
	}
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

void PhysicsSystem::moveAndCollide(const unsigned int id, const Vector2f& delta) {
	BodyPtr body = this->mBodies.at(id);
	this->handleCollision(delta, body, false);
}

Vector2fPtr PhysicsSystem::moveAndSlide(const unsigned int id, const Vector2f& delta) {
	BodyPtr body = this->mBodies.at(id);
	return std::make_shared<Vector2f>(this->handleCollision(delta, body, true));
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
		}
	}

	LuaScriptSystemPtr scriptSystem = mSystemManager->getSystemByType<LuaScriptSystem>(SystemType::LUA_SCRIPT);
	for (auto entry : scriptSystem->getLuaScripts()) {
		if (entry.second->state["onPhysics"].exists()) {
			entry.second->invoke("onPhysics", delta);
		}
	}
}
Sweep PhysicsSystem::sweep(const Vector2f& position, const Vector2f& delta, BodyPtr body) {
	// Handle overlap
	vector<BodyPtr> bodies;

	// project body to potential new position
	Vector2f oldPos(body->position);
	body->collider->colliderShape->position->set(position + delta);
	this->quadTree->getCollidingBodies(body, bodies);
	// remove position update so we can sweep the body for collisions.
	body->collider->colliderShape->position->set(oldPos);

	Vector2f newPosition(position + delta);

	Sweep sweep;
	sweep.time = 1.0f;
	sweep.position->set(newPosition);
	for (auto bod : bodies) {
		auto bodPtr = bod;
		Sweep currentSweep;
		bodPtr->collider->colliderShape->sweep(body->collider->colliderShape, delta, &currentSweep);
		if (currentSweep.time < sweep.time) {
			sweep = currentSweep;
		}

		EntityCollisionEventData* eventData = GCC_NEW EntityCollisionEventData(body->id, bodPtr->id, SDL_GetTicks());
		EventManager::getInstance().pushEvent(eventData);
	}

	return sweep;
}

Vector2f PhysicsSystem::handleCollision(const Vector2f& delta, BodyPtr body, bool slide) {
	if (delta.x == 0.0f && delta.y == 0.0f) {
		return Vector2f(0.0f, 0.0f);
	}

	Vector2f position(body->getPosition());
	Sweep sweep = this->sweep(position, delta, body);

	Vector2f velocity(delta);
	Vector2f finalPos = *sweep.position;
	if (sweep.time < 1.0f && slide) {
		Vector2f normal(sweep.manifold->normal);
		Vector2f slideDelta = velocity.slide(normal);
		slideDelta.normalize();
		slideDelta *= (1.0f - sweep.time);
		body->position->set(finalPos);
		sweep = this->sweep(finalPos, slideDelta, body);
		slideDelta *= sweep.time;
		finalPos += slideDelta;
	}

	body->position->set(finalPos);
	quadTree->removeBody(body);
	quadTree->addBody(body);

	return Vector2f(sweep.manifold->normal);
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
	keyEvent->text = evt.text.text;
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
		case SDL_TEXTINPUT: {
			type = InputEventType::ON_TEXT_INPUT;

			KeyEventData* keyDownEventData = GCC_NEW KeyEventData(SDL_GetTicks(), keyEvent->key, KeyAction::DOWN, keyEvent->ctrlDown, keyEvent->shiftDown);
			keyDownEventData->text += evt.text.text;
			EventManager::getInstance().pushEvent(keyDownEventData);

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

bool DefaultMouseMovementHandler::translateToRelative(unsigned long id, const Vector2f& position, Vector2f& relative) {
	GraphicsSystemPtr graphicsSystem = mSystemManager->getSystemByType<GraphicsSystem>(SystemType::GRAPHICS);
	EntitySystemPtr entitySystem = mSystemManager->getSystemByType<EntitySystem>(SystemType::ENTITY);
	auto ent = entitySystem->getEntityById(id);

	Components& components = ent->getComponents();
	if (components.find(ComponentType::PHYSICS_COMPONENT) == components.end()) {
		return false;
	}

	PhysicsComponentPtr physicsComponent = ent->getComponentByType<PhysicsComponent>(ComponentType::PHYSICS_COMPONENT);
	Vector2f entPos(physicsComponent->getPosition());
	
	// if isn't a ui element lets translate to sceen space
	if (components.find(ComponentType::BUTTON_COMPONENT) != components.end()
		&& components.find(ComponentType::ITEM_PANEL_COMPONENT) != components.end()
		&& components.find(ComponentType::LABEL_COMPONENT) != components.end()
		&& components.find(ComponentType::TEXT_BOX_COMPONENT) != components.end()) {

		CameraPtr cam = makeShared(graphicsSystem->getCamera());

		entPos -= cam->position;
	}

	relative = entPos - position;

	return true;
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

void SoundSystem::playMusic(const string& assetTag, int loop) {
	AssetSystemPtr assetSystem = mSystemManager->getSystemByType<AssetSystem>(SystemType::ASSET);
	AssetPtr asset(assetSystem->getAsset(assetTag));
	shared_ptr<Mix_Music> mix(asset->getAsset<Mix_Music>()); 
	Mix_PlayMusic(mix.get(), loop);
	Mix_VolumeMusic(32);
}

void SoundSystem::playSound(const string& assetTag, int channel, int loop) {
	AssetSystemPtr assetSystem = mSystemManager->getSystemByType<AssetSystem>(SystemType::ASSET);
	AssetPtr asset(assetSystem->getAsset(assetTag));
	shared_ptr<Mix_Chunk> mix = asset->getAsset<Mix_Chunk>();
	Mix_Volume(channel, 32);
	Mix_PlayChannel(channel, mix.get(), loop);
}

void SoundSystem::stopMusic(const string& assetTag) {
	AssetSystemPtr assetSystem = mSystemManager->getSystemByType<AssetSystem>(SystemType::ASSET);
	AssetPtr asset(assetSystem->getAsset(assetTag));
	shared_ptr<Mix_Music> mix(asset->getAsset<Mix_Music>());
	Mix_HaltMusic();
}

void SoundSystem::stopSound(const string& assetTag, int channel) {
	Mix_HaltChannel(channel);
}

void SoundSystem::pause(const string& assetTag) {

}

WorldSystem::WorldSystem(SystemManagerPtr systemManager) : System(SystemType::WORLD, systemManager) {}

void WorldSystem::loadWorld(string& path) {
	std::fstream file;
	file.open(path, std::ios::in);
	if (!file.is_open()) {
		ERR("Unable to open file: " + path);
	}
	std::string line;
	std::string builder;
	while (std::getline(file, line)) {
		builder.append(line);
	}
	file.close();

	rapidjson::Document doc;
	doc.Parse(builder.c_str());

	WorldPtr world = std::make_shared<World>((const rapidjson::Value&) doc);
	this->addWorld(world);
}

void WorldSystem::addWorld(WorldPtr world) {

	this->world = world;
}

WorldPtr WorldSystem::getWorld() {
	return this->world;
}

void WorldSystem::destroyWorld() {
	EntitySystemPtr entitySystem = mSystemManager->getSystemByType<EntitySystem>(SystemType::ENTITY);

	vector<unsigned int> ids;

	for (auto ent : world->entities) {
		ids.push_back(ent->id);
	}

	for (auto ent : ids) {
		entitySystem->deregisterEntity(ent);
	}

	for (auto row : world->grids) {
		for (auto grid : row) {
			entitySystem->deregisterEntity(grid->id);
		}
	}

	if (world->script != nullptr) {
		entitySystem->deregisterEntity(world->script->id);
	}
}

void WorldSystem::registerEntityAndComponents(EntityPtr entity) {

	for (auto component : entity->getComponents()) {
		switch (component.first) {
			case ComponentType::DRAWABLE_COMPONENT: {
				GraphicsSystemPtr graphicsSystem = mSystemManager->getSystemByType<GraphicsSystem>(SystemType::GRAPHICS);
				DrawableComponentPtr drawableComponent = entity->getComponentByType<DrawableComponent>(ComponentType::DRAWABLE_COMPONENT);
				graphicsSystem->registerDrawable(entity->id, drawableComponent->getDrawable());
				break;
			}
			case ComponentType::PHYSICS_COMPONENT: {
				PhysicsSystemPtr physicsSystem = mSystemManager->getSystemByType<PhysicsSystem>(SystemType::PHYSICS);
				PhysicsComponentPtr physicsComponent = entity->getComponentByType<PhysicsComponent>(ComponentType::PHYSICS_COMPONENT);
				physicsSystem->registerBody(entity->id, physicsComponent->getBody());
				break;
			}
			case ComponentType::ANIMATION_COMPONENT: {
				GraphicsSystemPtr graphicsSystem = mSystemManager->getSystemByType<GraphicsSystem>(SystemType::GRAPHICS);
				AnimationSystemPtr animationSystem = mSystemManager->getSystemByType<AnimationSystem>(SystemType::ANIMATION);
				AnimationComponentPtr animationComponent = entity->getComponentByType<AnimationComponent>(ComponentType::ANIMATION_COMPONENT);
				graphicsSystem->registerDrawable(entity->id, animationComponent->getAnimationDrawable());
				animationSystem->registerAnimation(entity->id, animationComponent->getAnimationDrawable()->animationHandler);
				break;
			}
			case ComponentType::LUA_SCRIPT_COMPONENT: {
				LuaScriptSystemPtr scriptSystem = mSystemManager->getSystemByType<LuaScriptSystem>(SystemType::LUA_SCRIPT);
				LuaScriptComponentPtr scriptComponent = entity->getComponentByType<LuaScriptComponent>(ComponentType::LUA_SCRIPT_COMPONENT);
				scriptSystem->registerLuaScript(entity->id, scriptComponent->script);
				break;
			}
			case ComponentType::PARTICLE_COMPONENT: {
				GraphicsSystemPtr graphicsSystem = mSystemManager->getSystemByType<GraphicsSystem>(SystemType::GRAPHICS);
				ParticleSystemPtr particleSystem = mSystemManager->getSystemByType<ParticleSystem>(SystemType::PARTICLE);
				ParticleCloudComponentPtr particleComponent = entity->getComponentByType<ParticleCloudComponent>(ComponentType::PARTICLE_COMPONENT);
				graphicsSystem->registerDrawable(entity->id, particleComponent->particleCloudDrawable);
				particleSystem->registerParticleEmitter(entity->id, particleComponent->particleCloudDrawable);
				break;
			}
			case ComponentType::GRID_COMPONENT: {
				GraphicsSystemPtr graphicsSystem = mSystemManager->getSystemByType<GraphicsSystem>(SystemType::GRAPHICS);
				GridComponentPtr gridComponent = entity->getComponentByType<GridComponent>(ComponentType::GRID_COMPONENT);
				graphicsSystem->registerDrawable(entity->id, gridComponent->getGridDrawable());
				break;
			}
		}
	}
}