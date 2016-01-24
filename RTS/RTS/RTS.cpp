#include"RTS.h"

void RTS::setup() {
	if (TTF_Init() < 0) {
		throw std::exception("Failed to initialize TTF");
	}

	if (SDL_Init(SDL_INIT_VIDEO) < 0) {
		throw std::exception("Error setting up SDL.");
	}

	mWindow.reset(
		SDL_CreateWindow(
		"Project RTS",
		SDL_WINDOWPOS_UNDEFINED,
		SDL_WINDOWPOS_UNDEFINED,
		800,
		600,
		SDL_WINDOW_SHOWN)
		);

	mRenderer.reset(
		SDL_CreateRenderer(mWindow.get(), -1, SDL_RENDERER_ACCELERATED),
		[](SDL_Renderer* renderer) {SDL_DestroyRenderer(renderer); }
		);

	TTF_Font* font(TTF_OpenFont("Digital_tech.otf", 11));
	mFont.reset(font, [](TTF_Font* font) { TTF_CloseFont(font); });

	mCamera = std::shared_ptr<Camera>(new Camera{ 0, 0, 800, 600 });

	std::vector<TileType> map{};
	for (int j = 0; j < 20; j++) {
		for (int i = 0; i < 20; i++) {
			if (j == 10) {
				map.push_back(DIRT);
			}
			else {
				map.push_back(GRASS);
			}
		}
	}

	mMap.reset(new Map(20, 20, map, mCamera));

	createUnit();
}

void RTS::handleEvents()
{
	SDL_Event event;
	while (SDL_PollEvent(&event)) {
		if (event.type == SDL_QUIT) {
			mIsPlaying = false;
		}
		if (event.type == SDL_KEYDOWN) {
			if (event.key.keysym.sym == SDLK_LEFT) {
				mCamera->x -= 10;
			}
			else if (event.key.keysym.sym == SDLK_RIGHT) {
				mCamera->x += 10;
			}
			else if (event.key.keysym.sym == SDLK_UP) {
				mCamera->y -= 10;
			}
			else if (event.key.keysym.sym == SDLK_DOWN) {
				mCamera->y += 10;
			}
		}
		if (event.type == SDL_MOUSEBUTTONUP) {
			int x, y;
			SDL_GetMouseState(&x, &y);

			bool plantedTower = false;
			std::shared_ptr<Tile> tile{ nullptr };
			for (int j = 0; j < 20; j++) {
				for (int i = 0; i < 20; i++) {
					auto tileAt = mMap->getTileAt(i, j);
					if (tileAt->checkPoint(vector2f(x, y)) && tileAt->canOccupy()) {
						tile = tileAt;
						plantedTower = true;
						break;
					}
				}

				if (plantedTower) {
					break;
				}
			}

			if (event.button.button == SDL_BUTTON_LEFT) {
				if (tile != nullptr) {
					std::shared_ptr<Tower> newTower(new Tower(mCamera, &mUnits));
					mTowers.push_back(newTower);
					tile->setOccupied(true);
					newTower->setLocation((vector2f&)tile->getLocation());
				}
			}
			else {
				if (mUnits[0]->getNumStates() == 0) {
					mField.reset(new Flowfield(tile, mMap, mFont, mRenderer));

					std::shared_ptr<PathState> newState(new PathState(mUnits[0], mField, &mTowers));
					newState->begin();
					mUnits[0]->pushState(newState);
				}
			}
		}
	}
}

void RTS::update(){
	auto iterator = mUnits.begin();
	while (iterator != mUnits.end()) {
		auto unit = iterator->get();
		unit->update();
		iterator++;
	}
	/*
	for (auto tower : mTowers) {
		tower->update();
	}*/
}

void RTS::draw(){
	SDL_SetRenderDrawColor(mRenderer.get(), 0, 0, 0, 0);
	SDL_RenderClear(mRenderer.get());

	mMap->draw(mRenderer);

	for (auto tower : mTowers) {
		tower->draw(mRenderer);
	}

	for (auto unit : mUnits) {
		unit->draw(mRenderer);
	}

	if (mField != nullptr) {
		mField->debugDraw(mRenderer, mCamera);
	}

	SDL_RenderPresent(mRenderer.get());
}
void RTS::delay(Uint32 frameTime){
	if (frameTime < 33) {
		SDL_Delay(Uint32(33) - frameTime);
	}
}

void RTS::teardown(){
	TTF_Quit();
	SDL_Quit();
}

void RTS::createUnit() {
	std::shared_ptr<Unit> newUnit(new Unit(100, 100, TILE_WIDTH, TILE_HEIGHT, 255, 0, 0, WARRIOR, mFont, mRenderer, mCamera));
	mUnits.push_back(newUnit);

	std::shared_ptr<Tile> tileAt = mMap->getTileAt(10, 10);
	newUnit->setCurrentTile(tileAt);
	newUnit->setLocation((vector2f&)tileAt->getLocation());
}