#ifndef __FLOWFIELD_H__
#define __FLOWFIELD_H__

#include<queue>
#include<SDL_ttf.h>

#include"Map.h"
#include"SDL_Helpers.h"
#include"vector2f.h"

class Flowfield {

public:
	Flowfield(Tile* target, Map* map) {
		mTargetTile = target;
		mMap = map;

		recalculate();
	}

	void recalculate() {
		int targetX = mTargetTile->xIndex;
		int targetY = mTargetTile->yIndex;
		
		std::list<Tile*> openList({ mTargetTile });
		std::unordered_set<Tile*> closedList;

		mCostMap[mTargetTile->getId()] = 0;

		while (openList.size() > 0) {
			auto currentTile = openList.front();
			openList.pop_front();
			closedList.emplace(currentTile);
			int currentCost = mCostMap[currentTile->getId()];

			for (int j = -1; j <= 1; j++) {
				for (int i = -1; i <= 1; i++) {
					int x = currentTile->xIndex + i;
					int y = currentTile->yIndex + j;
					// check bounds, and strip out diagonals and the original coordinates.
					if (x < 0 || x >= mMap->getMapWidth() || y < 0 || y >= mMap->getMapHeight() || (i == 0 && j == 0) || (std::abs(i) == std::abs(j))) {
						continue;
					}
					auto neighborTile = mMap->getTileAt(x, y);

					if (closedList.find(neighborTile) != closedList.end() || std::find(openList.begin(), openList.end(), neighborTile) != openList.end() || !neighborTile->canOccupy()) {
						if (!neighborTile->canOccupy()) {
							mCostMap[neighborTile->getId()] = 255;
						}
						continue;
					}

					mCostMap[neighborTile->getId()] = currentCost + 1;
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
				int costValue = mCostMap[currentTile->getId()];
				std::string cost = std::to_string(costValue);


				if (!currentTile->canOccupy()) {
					continue;
				}

				vector2f nextDir{ 0, 0 };
				if (currentTile == mTargetTile) {
					mVectorMap[currentTile->getId()] = std::unique_ptr<vector2f>(new vector2f(nextDir));
					continue;
				}


				int shortest = 255;
				for (auto dir : directions) {
					int xIndex = currentTile->xIndex + dir.x;
					int yIndex = currentTile->yIndex + dir.y;
					if (xIndex < 0 || xIndex >= mMap->getMapWidth() || yIndex < 0 || yIndex >= mMap->getMapWidth()) {
						continue;
					}

					auto tileAt = mMap->getTileAt(xIndex, yIndex);
					int cost = mCostMap[tileAt->getId()];
					if (cost < shortest) {
						shortest = cost;
						nextDir = dir;
					}
				}

				mVectorMap[currentTile->getId()] = std::unique_ptr<vector2f>(new vector2f(nextDir));
				mVectorMap[currentTile->getId()]->normalize();
			}
		}
	}

	Tile* tileAtPoint(const vector2f* point) {
		int xIndex = std::round(point->x / float(TILE_WIDTH));
		int yIndex = std::round(point->y / float(TILE_HEIGHT));

		return mMap->getTileAt(xIndex, yIndex);
	}

	vector2f* getVectorForTile(unsigned const long tileId) const {
		return mVectorMap.at(tileId).get();
	}

	Tile* mTargetTile;
private:
	Map* mMap;
	// map from tile indicies to next closest tile with the shortest path.
	std::unordered_map<unsigned long, std::unique_ptr<vector2f>> mVectorMap;
	std::unordered_map<unsigned long, short> mCostMap;
};

#endif // !__FLOWFIELD_H__
