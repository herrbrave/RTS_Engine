#include"PathState.h"

PathState::PathState(std::shared_ptr<Unit> unit, std::shared_ptr<Flowfield> flowField, std::vector<std::shared_ptr<Tower>> * towers) {
	mUnit = unit;
	mFlowfield = flowField;
	mTowers = towers;
}

void PathState::begin() {
	mStarted = true;
}

void PathState::update() {
	if (!mStarted || mEnded) {
		return;
	}

	auto tile = mFlowfield->tileAtPoint(mUnit->getLocation());
	if (tile == nullptr || tile == mFlowfield->mTargetTile) {
		end();
		return;
	}

	auto vector = mFlowfield->getVectorForTile(tile);
	if (vector == nullptr) {
		end();
		return;
	}
	vector2f currentLocation(mUnit->getLocation());
	vector2f velocity((*vector) * float(2));
	vector2f nextLocation(currentLocation + velocity);
	vector2f velocityCopy(velocity);
	velocityCopy.normalize();
	vector2f seeAhead((velocityCopy * MAX_SEE_AHEAD) + nextLocation);

	mUnit->setLocation(seeAhead);
	bool collisionFound(false);
	for (auto tower : *mTowers) {
		if (tower->checkCollision(*mUnit)) {
			vector2f delta(seeAhead - tower->getLocation());
			delta.normalize();
			velocity.normalize();
			velocity = velocity + delta;
			velocity.normalize();
			velocity = velocity * float(2);
			nextLocation = velocity + currentLocation;
			break;
		}
	}

	if (!collisionFound) {
		mUnit->setLocation(nextLocation);
	}
}

void PathState::end() {
	mEnded = true;
}

bool PathState::hasBegun() {
	return mStarted;
}

bool PathState::isEnded() {
	return mEnded;
}