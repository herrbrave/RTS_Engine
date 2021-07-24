#ifndef __FLOWFIELD_H__
#define __FLOWFIELD_H__

#include<queue>
#include<SDL_ttf.h>

#include"Map.h"
#include"SDL_Helpers.h"
#include"Vector2f.h"

class Flowfield;
typedef shared_ptr<Flowfield> FlowfieldPtr;
typedef weak_ptr<Flowfield> WeakFlowfieldPtr;

class Flowfield {

public:
	Flowfield(EntityPtr target, MapPtr map) {
		mTargetTile = target;
		mMap = map;

		recalculate();
	}

	void recalculate() {
		shared_ptr<TileComponent> tileComponent = mTargetTile->getComponentByType<TileComponent>(ComponentType::TILE_COMPONENT);
		if (tileComponent == nullptr) {
			throw GCC_NEW std::exception("Failed to get tile component from weak pointer.");
		}

		int targetX = tileComponent->x;
		int targetY = tileComponent->y;
		
		std::list<EntityPtr> openList({mTargetTile});
		std::unordered_set<EntityPtr> closedList;

		mCostMap[mTargetTile->id] = 0;

		while (openList.size() > 0) {
			auto currentTile = openList.front();
			tileComponent = currentTile->getComponentByType<TileComponent>(ComponentType::TILE_COMPONENT);
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
					auto neighborTileComponent = neighborTile->getComponentByType<TileComponent>(ComponentType::TILE_COMPONENT);

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

		std::vector<Vector2f> directions {
			Vector2f{-1, 0 },
			Vector2f{ -1, 1 },
			Vector2f{ 0, 1 },
			Vector2f{ 1, 1 },
			Vector2f{ 1, 0 },
			Vector2f{ 1, -1 },
			Vector2f{ 0, -1 },
			Vector2f{ -1, -1 }
		};

		for (int y = 0; y < mMap->getMapHeight(); y++) {
			for (int x = 0; x < mMap->getMapWidth(); x++) {
				auto currentTile = mMap->getTileAt(x, y);
				tileComponent = currentTile->getComponentByType<TileComponent>(ComponentType::TILE_COMPONENT);
				int costValue = mCostMap[currentTile->id];
				std::string cost = std::to_string(costValue);


				if (!tileComponent->canOccupy) {
					continue;
				}

				Vector2f nextDir{ 0, 0 };
				if (currentTile->id == mTargetTile->id) {
					mVectorMap[currentTile->id] = std::unique_ptr<Vector2f>(new Vector2f(nextDir));
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
						nextDir.set(dir);
					}
				}

				mVectorMap[currentTile->id] = std::unique_ptr<Vector2f>(new Vector2f(nextDir));
				mVectorMap[currentTile->id]->normalize();
			}
		}
	}

	void guideEntity(EntityPtr entity) {
		auto physicsComponent = entity->getComponentByType<PhysicsComponent>(ComponentType::PHYSICS_COMPONENT);
		EntityPtr tileAt = tileAtPoint(physicsComponent->getPosition());
		auto velocity = getVectorForTile(tileAt->id);

		physicsComponent->setVelocity(velocity);
	}

	bool tileExistsAtPoint(const Vector2f& point) {
		int xIndex = std::round(point.x / float(mMap->getTileWidth()));
		int yIndex = std::round(point.y / float(mMap->getTileHeight()));

		return mMap->tileExistsAtPoint(xIndex, yIndex);
	}

	EntityPtr tileAtPoint(const Vector2f& point) {
		return mMap->tileAtPoint(point);
	}

	const Vector2f& getVectorForTile(unsigned const long tileId) const {
		if (mVectorMap.find(tileId) == mVectorMap.end()) {
			return Vector2f(0, 0);
		}
		return mVectorMap.at(tileId);
	}

	EntityPtr mTargetTile;
private:
	MapPtr mMap;
	// map from tile indicies to next closest tile with the shortest path.
	std::unordered_map<unsigned long, Vector2fPtr> mVectorMap;
	std::unordered_map<unsigned long, short> mCostMap;
};

#endif // !__FLOWFIELD_H__
