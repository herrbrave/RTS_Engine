#include"Tower.h"

void Tower::draw(std::shared_ptr<SDL_Renderer> renderer) {
	Entity::draw(renderer);
	for (auto bullet : mBullets) {
		bullet->draw(renderer);
	}

	if (mBuilding) {
		mBuildProgress->draw(renderer.get(), &mBlock->getRect(mCamera));
	}
}

void Tower::update() {
	Entity::update();

	if (mBuilding) {
		Uint32 currentTime = SDL_GetTicks();
		if ((currentTime - mLastBuildingProgress) > 100) {
			mBuildProgress->setProgress(mBuildProgress->getProgress() + 1);
			mLastBuildingProgress = currentTime;

			if (mBuildProgress->getProgress() == mBuildProgress->getMaxProgress()) {
				mBuilding = false;

				mBlock->m_a = 255;
			}
		}

		return;
	}

	auto bulletIterator = mBullets.begin();
	while (bulletIterator != mBullets.end()) {
		if (bulletIterator->get()->getNumStates() == 0) {
			bulletIterator = mBullets.erase(bulletIterator);
		}
		else {
			bulletIterator->get()->update();
			bulletIterator++;
		}
	}

	Uint32 currentTime = SDL_GetTicks();
	if ((currentTime - mLastGeneration) < 500) {
		return;
	}

	mLastGeneration = currentTime;

	std::shared_ptr<Unit> closestUnit = nullptr;
	for (auto unit : *mUnits) {
		float currentDistance = distance(unit);
		if (currentDistance < 225) {
			closestUnit = unit;
			break;
		}
	}

	if (closestUnit != nullptr) {
		createBullet(closestUnit);
	}
}

void Tower::createBullet(std::shared_ptr<Unit> target) {
	std::shared_ptr<Entity> bullet(new Entity(getLocation()[0], getLocation()[1], 8, 8, 255, 255, 255, mCamera));
	mBullets.push_back(bullet);

	bullet->pushState(std::shared_ptr<ShootState>(new ShootState(std::shared_ptr<Entity>(bullet), target)));
}