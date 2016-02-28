#ifndef __FLOWFIELD_H__
#define __FLOWFIELD_H__

#include<queue>
#include<SDL_ttf.h>

#include"Map.h"
#include"SDL_Helpers.h"
#include"vector2f.h"

class Flowfield {

public:
	Flowfield(Entity* target, Map* map) {
		mTargetTile = target;
		mMap = map;

		recalculate();
	}

	void recalculate() {
		auto tileComponent = reinterpret_cast<TileComponent*>(mTargetTile->componentContainer->getComponentByType(ComponentType::TILE_COMPONENT));
		int targetX = tileComponent->x;
		int targetY = tileComponent->y;
		
		std::list<Entity*> openList({ mTargetTile });
		std::unordered_set<Entity*> closedList;

		mCostMap[mTargetTile->id] = 0;

		while (openList.size() > 0) {
			auto currentTile = openList.front();
			tileComponent = reinterpret_cast<TileComponent*>(currentTile->componentContainer->getComponentByType(ComponentType::TILE_COMPONENT));
			openList.pop_front();
			closedList.emplace(currentTile);
			int currentCost = mCostMap[currentTile->id];

			for (int j = -1; j <= 1; j++) {
				for (int i = -1; i <= 1; i++) {
					int x = tileComponent->x + i;
					int y = tileComponent->y + j;
					// check bounds, and strip out diagonals and the original coordinates.
					if (x < 0 || x >= mMap->getMapWidth() || y < 0 || y >= mMap->getMapHeight() || (i == 0 && j == 0) || (std::abs(i) == std::abs(j))) {
						continue;
					}
					auto neighborTile = mMap->getTileAt(x, y);
					auto neighborTileComponent = reinterpret_cast<TileComponent*>(neighborTile->componentContainer->getComponentByType(ComponentType::TILE_COMPONENT));

					if (closedList.find(neighborTile) != closedList.end() || std::find(openList.begin(), openList.end(), neighborTile) != openList.end()) {
						if (!neighborTileComponent->canOccupy) {
							mCostMap[neighborTile->id] = 255;
						}
						continue;
					}

					mCostMap[neighborTile->id] = currentCost + 1;
					openList.push_back(neighborTile);
				}
			}
		}

		std::vector<vector2f> directions {
			vector2f{-1, 0 },
			vector2f{ -1, 1 },
			vector2f{ 0, 1 },
			vector2f{ 1, 1 },
			vector2f{ 1, 0 },
			vector2f{ 1, -1 },
			vector2f{ 0, -1 },
			vector2f{ -1, -1 }
		};

		for (int y = 0; y < mMap->getMapHeight(); y++) {
			for (int x = 0; x < mMap->getMapWidth(); x++) {
				auto currentTile = mMap->getTileAt(x, y);
				tileComponent = reinterpret_cast<TileComponent*>(currentTile->componentContainer->getComponentByType(ComponentType::TILE_COMPONENT));
				int costValue = mCostMap[currentTile->id];
				std::string cost = std::to_string(costValue);


				if (!tileComponent->canOccupy) {
					continue;
				}

				vector2f nextDir{ 0, 0 };
				if (currentTile == mTargetTile) {
					mVectorMap[currentTile->id] = std::unique_ptr<vector2f>(new vector2f(nextDir));
					continue;
				}


				int shortest = 255;
				for (auto dir : directions) {
					int xIndex = tileComponent->x + dir.x;
					int yIndex = tileComponent->y + dir.y;
					if (xIndex < 0 || xIndex >= mMap->getMapWidth() || yIndex < 0 || yIndex >= mMap->getMapWidth()) {
						continue;
					}

					auto tileAt = mMap->getTileAt(xIndex, yIndex);
					int cost = mCostMap[tileAt->id];
					if (cost < shortest) {
						shortest = cost;
						nextDir = dir;
					}
				}

				mVectorMap[currentTile->id] = std::unique_ptr<vector2f>(new vector2f(nextDir));
				mVectorMap[currentTile->id]->normalize();
			}
		}
	}

	Entity* tileAtPoint(const vector2f* point) {
		return mMap->tileAtPoint(point);
	}

	vector2f* getVectorForTile(unsigned const long tileId) const {
		return mVectorMap.at(tileId).get();
	}

	Entity* mTargetTile;
private:
	Map* mMap;
	// map from tile indicies to next closest tile with the shortest path.
	std::unordered_map<unsigned long, std::unique_ptr<vector2f>> mVectorMap;
	std::unordered_map<unsigned long, short> mCostMap;
};

#endif // !__FLOWFIELD_H__
