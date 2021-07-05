#include"MapFactory.h"

EntityPtr TileFactory::createTile(const string& assetTag, int xIndex, int yIndex, const Vector2f& position, float tx, float ty, float width, float height) {
	EntityPtr entity = createTexturedEntity(assetTag, position.x, position.y, width, height, tx, ty, width, height, false);

	TileComponentPtr tileComponent(GCC_NEW TileComponent(entity->id, xIndex, yIndex));
	entity->addComponent(tileComponent);

	return entity;
}

MapFactory::MapFactory(TileFactoryPtr tileFactory, LuaScriptFactoryPtr luaScriptFactory, SystemManagerPtr systemManager) {
	mTileFactory = tileFactory;
	mSystemManager = systemManager;
	mLuaScriptFactory = luaScriptFactory;
}

MapPtr MapFactory::createMap(const string& pathToMap) {
	TMXMapPtr tmxMap = parseMap(pathToMap);

	Tileset tileset;
	TileAnimationSet animations;
	auto tilesets = tmxMap->tilesets;
	for (TMXTilesetPtr _tileset : tilesets) {
		this->loadGridTileset(_tileset, tileset, animations);
	}

	MapConfig* mapConfig = GCC_NEW MapConfig();
	mapConfig->mapWidth = tmxMap->width;
	mapConfig->mapHeight = tmxMap->height;
	mapConfig->tileWidth = tmxMap->tileWidth;
	mapConfig->tileHeight = tmxMap->tileHeight;
	mapConfig->tileset = tileset;
	mapConfig->animatedTiles = animations;

	auto layers = tmxMap->layers;
	int index = 0;
	for (TMXLayerPtr layer : layers) {
		std::string type = layer->type;
		if (type == "tilelayer") {
			loadTileLayer(layer, tmxMap->width, tmxMap->height, tmxMap->tileWidth, tmxMap->tileHeight, *mapConfig, index);
		}
		else {
			loadObjectLayer(layer, *mapConfig, index);
		}
		index++;
	}

	// TODO: provide a factory for the entity vendor, or inject it into the map factory.
	EntitySystemPtr entitySystem(mSystemManager->getSystemByType<EntitySystem>(SystemType::ENTITY));
	MapPtr map(GCC_NEW Map(MapConfigPtr(mapConfig), EntityVendorPtr(GCC_NEW EntitySystem::DefaultEntityVendor(entitySystem))));

	PhysicsSystemPtr physicsSystem = makeShared(mSystemManager->getSystemByType<PhysicsSystem>(SystemType::PHYSICS));
	physicsSystem->setWorldSize(map->getMapWidth() * map->getTileWidth(), map->getMapHeight() * map->getTileHeight());

	MapLoadedSetEventData* eventData = GCC_NEW MapLoadedSetEventData(SDL_GetTicks());
	EventManager::getInstance().pushEvent(eventData);

	if (!tmxMap->properties.empty()) {
		auto properties = tmxMap->properties;
		for (TMXPropertyPtr prop : properties) {
			if (string("script").compare(prop->name) == 0) {
				auto ptr = prop->getValue<char*>();
				auto shPtr = makeShared(ptr);
				auto c = (char*)shPtr.get();
				string script(c);
				mTileFactory->createScriptEntity(script);
			}
		}
	}

	return map;
}

void MapFactory::loadGridTileset(const TMXTilesetPtr& tileset, Tileset& tiles, TileAnimationSet& animations) {
	// If the tileset contains an image then it is a grid based tileset.
	if (!tileset->image.empty()) {
		string image = tileset->image;

		GraphicsSystemPtr graphicsSystem = static_pointer_cast<GraphicsSystem>(mSystemManager->systems.at(SystemType::GRAPHICS));
		graphicsSystem->addTexture(image, image);

		int tileWidth = tileset->tileWidth;
		int tileHeight = tileset->tileHeight;
		int columns = tileset->columns;
		int tileCount = tileset->tileCount;
		int id = tileset->firstgid;
		int count = 0;
		int row = 0;

		while (count < tileCount) {
			for (int i = 0; i < columns && count < tileCount; i++, count++) {
				tiles.emplace(id, TilePtr(GCC_NEW Tile(image, i * tileWidth, row * tileHeight, tileHeight, tileWidth))); 
				id++;
			}
			row++;
		}

		id = tileset->firstgid;
		auto tilesArray = tileset->tiles;
		for (TMXTilePtr tile : tilesArray) {
			TilePtr newTile = tiles[id++];
			if (!tile->objectgroup.empty()) {
				auto objectGroup = tile->objectgroup;
				auto collisionObject = objectGroup[0];
				newTile->collision = true;
				newTile->xOff = 0;
				newTile->yOff = 0;
				newTile->collisionWidth = collisionObject->width;
				newTile->collisionHeight = collisionObject->height;
			}

			if (!tile->animation.empty()) {
				AnimationSetPtr animationSet(GCC_NEW AnimationSet());
				id = tileset->firstgid;
				newTile->animated = true;
				animations[id] = animationSet;
				
				animationSet->spritesheet = image;
				animationSet->fps = 0;
				animationSet->defaultAnimationName = "default";
				animationSet->name = tileset->name;

				AnimationPtr anim(GCC_NEW Animation());
				animationSet->animations["default"] = anim;

				auto animation = tile->animation;
				for (TMXFramePtr anim : animation) {
					if (animationSet->fps == 0) {
						int frameTime = anim->duration;
						int fps = 1000 / frameTime;
						animationSet->fps = fps;
					}
					int tileId = anim->tileid + id;
					auto animTile = tiles[tileId];

					TexturePtr texture(GCC_NEW Texture(animTile->textureAssetTag, animTile->tx, animTile->ty, animTile->w, animTile->h));
					animationSet->animations["default"]->frames.push_back(texture);
				}
			}
		}
	}
	else {
		int id = tileset->firstgid;
		auto tilesArray = tileset->tiles;
		for (TMXTilePtr tile : tilesArray) {
			string image = tile->image;

			GraphicsSystemPtr graphicsSystem = static_pointer_cast<GraphicsSystem>(mSystemManager->systems.at(SystemType::GRAPHICS));
			graphicsSystem->addTexture(image, image);

			int tileWidth = tile->imageWidth;
			int tileHeight = tile->imageHeight;
			auto newTile = TilePtr(GCC_NEW Tile(image, 0, 0, tileHeight, tileWidth));
			tiles.emplace(id++, newTile);

			if (!tile->objectgroup.empty()) {
				auto objectGroup = tile->objectgroup;
				auto collisionObject = objectGroup[0];
				newTile->collision = true;
				newTile->xOff = 0;
				newTile->yOff = 0;
				newTile->collisionWidth = collisionObject->width;
				newTile->collisionHeight = collisionObject->height;
			}
		}
	}
}

void MapFactory::loadTileLayer(const TMXLayerPtr& layer, int width, int height, int tileWidth, int tileHeight, MapConfig& mapConfig, int drawOrder) {
	auto data = layer->data;
	PhysicsSystemPtr physicsSystem = makeShared(mSystemManager->getSystemByType<PhysicsSystem>(SystemType::PHYSICS));
	AnimationSystemPtr animationSystem = makeShared(mSystemManager->getSystemByType<AnimationSystem>(SystemType::ANIMATION));
	GraphicsSystemPtr graphicsSystem = makeShared(mSystemManager->getSystemByType<GraphicsSystem>(SystemType::GRAPHICS));
	for (int index = 0; index < layer->tileCount; index++) {
		int tileVal(data[index]);
		if (mapConfig.tileset.find(tileVal) == mapConfig.tileset.end()) {
			continue;
		}

		int x = (index % width);
		int y = (index / height);
		TilePtr t = mapConfig.tileset.at(tileVal);

		EntityPtr tile;
		if (t->animated) {
			auto animationSet = mapConfig.animatedTiles[tileVal];
			tile = mTileFactory->createPhysicsEntity(x * tileWidth, y * tileHeight, tileWidth, tileHeight); 
			TexturePtr texture(GCC_NEW Texture(""));
			shared_ptr<TextureDrawable> textureDrawable(GCC_NEW TextureDrawable(texture));
			graphicsSystem->registerDrawable(tile->id, textureDrawable);
			AnimationHandlerPtr animationHandler(GCC_NEW AnimationHandler(textureDrawable, animationSet, animationSet->fps));
			animationHandler->loop();
			animationSystem->registerAnimation(tile->id, animationHandler);
			AnimationComponentPtr animationComponent(GCC_NEW AnimationComponent(tile->id, animationHandler));

			TileComponentPtr tileComponent(GCC_NEW TileComponent(tile->id, x, y));
			tile->addComponent(tileComponent);
		}
		else {
			tile = mTileFactory->createTile(t->textureAssetTag, x, y, Vector2fPtr(GCC_NEW Vector2f(x * tileWidth, y * tileHeight)), t->tx, t->ty, t->w, t->h);
			auto drawableComponent = makeShared(tile->getComponentByType<DrawableComponent>(ComponentType::DRAWABLE_COMPONENT));
			//drawableComponent->setZOrder(drawOrder);
		}
		mapConfig.tiles.push_back(tile->id);


		TileComponentPtr tileComponent(tile->getComponentByType<TileComponent>(ComponentType::TILE_COMPONENT));
		// TODO: Add this value as a property of the tile.
		tileComponent->canOccupy = true;

		if (t->collision) {
			BodyPtr blockBody(GCC_NEW Body(tile->id, x, y, width, height));
			physicsSystem->registerBody(tile->id, blockBody);
			PhysicsComponentPtr physicsComponent(GCC_NEW PhysicsComponent(tile->id, blockBody));
			physicsComponent->setCollider(ColliderPtr(GCC_NEW Collider(GCC_NEW AABBColliderShape(std::make_shared<Vector2f>(x, y), t->collisionWidth, t->collisionHeight))));
		}
	}
}

void MapFactory::loadObjectLayer(const TMXLayerPtr& layer, MapConfig& mapConfig, int drawOrder) {
	auto data = layer->objects;
	for (TMXObjectPtr object : data) {
		int width = object->width;
		int height = object->height;
		int id = object->gid;

		// Tiled stored location from the top left corner.
		int x = (int)round(object->x);
		int y = (int)round(object->y) - height;

		string script = "";
		if (!object->properties.empty()) {
			auto properties = object->properties;
			for (TMXPropertyPtr prop : properties) {
				if (string("script").compare(prop->name) == 0) {
					auto ptr = prop->getValue<char*>();
					auto shPtr = makeShared(ptr);
					auto c = (char*)shPtr.get();
					script = string(c);
				}
			}
		}

		EntityPtr tile;
		if (id > 0) {
			TilePtr t = mapConfig.tileset.at(id);
			tile = mTileFactory->createPhysicsEntity(x, y, width, height);
			if (t->animated) {
				GraphicsSystemPtr graphicsSystem = makeShared(mSystemManager->getSystemByType<GraphicsSystem>(SystemType::GRAPHICS));
				AnimationSystemPtr animationSystem = makeShared(mSystemManager->getSystemByType<AnimationSystem>(SystemType::ANIMATION));
				tile = mTileFactory->createPhysicsEntity(x, y, width, height);
				TexturePtr texture(GCC_NEW Texture(""));
				shared_ptr<TextureDrawable> textureDrawable(GCC_NEW TextureDrawable(texture));
				textureDrawable->setDrawDepth(drawOrder);
				graphicsSystem->registerDrawable(tile->id, textureDrawable);
				AnimationHandlerPtr animationHandler(GCC_NEW AnimationHandler(textureDrawable, mapConfig.animatedTiles[id], mapConfig.animatedTiles[id]->fps));
				animationHandler->loop();
				animationSystem->registerAnimation(tile->id, animationHandler);
				AnimationComponentPtr animationComponent(GCC_NEW AnimationComponent(tile->id, animationHandler));

				TileComponentPtr tileComponent(GCC_NEW TileComponent(tile->id, x, y));
				tile->addComponent(tileComponent);
			}
			else {
				tile = mTileFactory->createTexturedEntity(t->textureAssetTag, x, y, width, height, t->tx, t->ty, t->w, t->h, true);
				auto drawableComponent = makeShared(tile->getComponentByType<DrawableComponent>(ComponentType::DRAWABLE_COMPONENT));
				drawableComponent->setZOrder(drawOrder);
			}
		}
		else {
			tile = mTileFactory->createPhysicsEntity(x, y, width, height);
		}

		mapConfig.objects.push_back(tile->id);

		if (script.size() > 0) {
			InputSystemPtr inputSystem(makeShared(mSystemManager->getSystemByType<InputSystem>(SystemType::INPUT)));
			InputListenerPtr inputListener(GCC_NEW InputListener(tile->id));
			inputSystem->registerEventListener(inputListener);

			LuaScriptPtr script(mLuaScriptFactory->create(script, tile->id));

			LuaScriptComponent* luaScriptComponent = GCC_NEW LuaScriptComponent(tile->id, script);
			tile->addComponent(ComponentPtr(luaScriptComponent));

			LuaScriptSystemPtr scriptSystem = makeShared<LuaScriptSystem>(mSystemManager->getSystemByType<LuaScriptSystem>(SystemType::LUA_SCRIPT));
			scriptSystem->registerLuaScript(tile->id, script);
		}
	}
}