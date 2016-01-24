#ifndef __FLOWFIELD_H__
#define __FLOWFIELD_H__

#include<queue>
#include<SDL_ttf.h>

#include"Map.h"
#include"SDL_Helpers.h"
#include"vector2f.h"

class Flowfield {

public:
	Flowfield(std::shared_ptr<Tile> target, std::shared_ptr<Map> map, std::shared_ptr<TTF_Font> font, std::shared_ptr<SDL_Renderer> renderer) {
		mTargetTile = target;
		mMap = map;

		mFont = font; 
		mRenderer = renderer;

		recalculate();
	}

	void recalculate() {
		int targetX = mTargetTile->mXLoc;
		int targetY = mTargetTile->mYLoc;
		
		std::list<std::shared_ptr<Tile>> openList({ mTargetTile });
		std::unordered_set<std::shared_ptr<Tile>> closedList;

		mCostMap[mTargetTile] = 0;

		while (openList.size() > 0) {
			auto currentTile = openList.front();
			openList.pop_front();
			closedList.emplace(currentTile);
			int currentCost = mCostMap[currentTile];

			for (int j = -1; j <= 1; j++) {
				for (int i = -1; i <= 1; i++) {
					int x = currentTile->mXLoc + i;
					int y = currentTile->mYLoc + j;
					// check bounds, and strip out diagonals and the original coordinates.
					if (x < 0 || x >= mMap->getMapWidth() || y < 0 || y >= mMap->getMapHeight() || (i == 0 && j == 0) || (std::abs(i) == std::abs(j))) {
						continue;
					}
					auto neighborTile = mMap->getTileAt(x, y);

					if (closedList.find(neighborTile) != closedList.end() || std::find(openList.begin(), openList.end(), neighborTile) != openList.end() || !neighborTile->canOccupy()) {
						if (!neighborTile->canOccupy()) {
							mCostMap[neighborTile] = 255;
						}
						continue;
					}

					mCostMap[neighborTile] = currentCost + 1;
					openList.push_back(neighborTile);
				}
			}
		}

		std::vector<std::shared_ptr<vector2f>> directions{
			std::shared_ptr<vector2f>(new vector2f(-1, 0)),
			std::shared_ptr<vector2f>(new vector2f(-1, 1)),
			std::shared_ptr<vector2f>(new vector2f(0, 1)),
			std::shared_ptr<vector2f>(new vector2f(1, 1)),
			std::shared_ptr<vector2f>(new vector2f(1, 0)),
			std::shared_ptr<vector2f>(new vector2f(1, -1)),
			std::shared_ptr<vector2f>(new vector2f(0, -1)),
			std::shared_ptr<vector2f>(new vector2f(-1, -1)),
		};

		for (int y = 0; y < mMap->getMapHeight(); y++) {
			for (int x = 0; x < mMap->getMapWidth(); x++) {
				auto currentTile = mMap->getTileAt(x, y);
				int costValue = mCostMap[currentTile];
				std::string cost = std::to_string(costValue);
				std::shared_ptr<SDL_Texture> costTexture(createText(cost, mFont.get(), mRenderer.get()), SDL_DELETERS());
				mPathDebug[currentTile] = costTexture;

				if (!currentTile->canOccupy()) {
					continue;
				}

				std::shared_ptr<vector2f> nextDir(new vector2f(0, 0));
				if (currentTile == mTargetTile) {
					mVectorMap[currentTile] = std::shared_ptr<vector2f>(nextDir);
					continue;
				}


				int shortest = 255;
				for (auto dir : directions) {
					int xIndex = currentTile->mXLoc + (*dir)[0];
					int yIndex = currentTile->mYLoc + (*dir)[1];
					if (xIndex < 0 || xIndex >= mMap->getMapWidth() || yIndex < 0 || yIndex >= mMap->getMapWidth()) {
						continue;
					}

					auto tileAt = mMap->getTileAt(xIndex, yIndex);
					int cost = mCostMap[tileAt];
					if (cost < shortest) {
						shortest = cost;
						nextDir = dir;
					}
				}

				mVectorMap[currentTile] = std::shared_ptr<vector2f>(new vector2f(*nextDir));
				mVectorMap[currentTile]->normalize();
			}
		}
	}

	std::vector<std::shared_ptr<Tile>> operator[](int x) {
		if (x < 0 || x >= mMap->getMapWidth()) {
			return std::vector < std::shared_ptr<Tile> > {};
		}

		std::vector<std::shared_ptr<Tile>> tiles;
		for (int y = 0; y < mMap->getMapHeight(); y++) {
			tiles.push_back(mMap->getTileAt(x, y));
		}

		return tiles;
	};

	SDL_Texture* createText(std::string text, TTF_Font* font, SDL_Renderer* renderer) {
		SDL_Color textColor = { 255, 255, 255, 255 };
		SDL_Surface* surface = TTF_RenderText_Solid(font, text.c_str(), textColor);
		SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
		SDL_FreeSurface(surface);

		return texture;
	}

	void debugDraw(std::shared_ptr<SDL_Renderer>renderer, std::shared_ptr<Camera> camera) {
		for (auto pair : mVectorMap) {
			auto tile = pair.first;
			auto vector = pair.second;
			if (vector == nullptr) {
				continue;
			}

			auto rect = tile->mBlock->getRect(camera);
			SDL_SetRenderDrawColor(renderer.get(), 255, 0, 0, 255);
			SDL_RenderDrawLine(renderer.get(), 
				rect.x, 
				rect.y,
				rect.x + ((TILE_WIDTH / 2) * (*vector)[0]),
				rect.y + ((TILE_WIDTH / 2) * (*vector)[1])
				);
		}

		for (auto entry : mPathDebug) {
			std::shared_ptr<SDL_Texture> texture = entry.second;

			SDL_Rect textRect{ 0, 0, 0, 0 };
			SDL_QueryTexture(texture.get(), nullptr, nullptr, &textRect.w, &textRect.h);

			SDL_Rect rect = entry.first->mBlock->getRect(camera);
			rect.x -= textRect.w / 2;
			rect.y -= textRect.h / 2;
			rect.w = textRect.w;
			rect.h = textRect.h;

			SDL_RenderCopy(renderer.get(), texture.get(), nullptr, &rect);
		}
	}

	std::shared_ptr<Tile> tileAtPoint(vector2f point) {
		int xIndex = std::round(point[0] / float(TILE_WIDTH));
		int yIndex = std::round(point[1] / float(TILE_HEIGHT));

		return mMap->getTileAt(xIndex, yIndex);
	}

	std::shared_ptr<vector2f> getVectorForTile(std::shared_ptr<Tile> tile) {
		return mVectorMap[tile];
	}

	std::shared_ptr<Tile> mTargetTile;
private:
	std::shared_ptr<Map> mMap;
	// map from tile indicies to next closest tile with the shortest path.
	std::unordered_map<std::shared_ptr<Tile>, std::shared_ptr<vector2f>> mVectorMap;
	std::unordered_map<std::shared_ptr<Tile>, short> mCostMap;

	std::shared_ptr<SDL_Renderer> mRenderer;
	std::shared_ptr<TTF_Font> mFont;
	std::unordered_map<std::shared_ptr<Tile>, std::shared_ptr<SDL_Texture>> mPathDebug;
};

#endif // !__FLOWFIELD_H__
