#ifndef __TOWER_H__
#define __TOWER_H__

#include<memory>
#include<vector>

#include"ShootState.h"
#include"Unit.h"

class Tower : public Entity {
public:
	Tower(std::shared_ptr<Camera> camera, std::vector<std::shared_ptr<Unit>>* units) : Entity(0, 0, TILE_WIDTH, TILE_HEIGHT, 0, 0, 128, camera) {
		mUnits = units;
		mBuildProgress.reset(new ProgressBar(100, TOP));
		mBuildProgress->setProgress(0);
		mBlock->m_a = 128;

		mLastBuildingProgress = SDL_GetTicks();
	}

	virtual void draw(std::shared_ptr<SDL_Renderer> renderer);
	virtual void update();
private:
	Uint32 mLastGeneration{ 0 };
	Uint32 mLastBuildingProgress{ 0 };
	std::shared_ptr<ProgressBar> mBuildProgress{ nullptr };
	std::vector<std::shared_ptr<Unit>>* mUnits;
	std::vector<std::shared_ptr<Entity>> mBullets;
	bool mBuilding{ true };

	void createBullet(std::shared_ptr<Unit> target);

	float distance(std::shared_ptr<Unit> target) {
		vector2f unitLocation = getLocation();
		vector2f targetLocation = target->getLocation();
		
		vector2f diff = unitLocation - targetLocation;

		return diff.magnitude();
	}
};

#endif // !__TOWER_H__
