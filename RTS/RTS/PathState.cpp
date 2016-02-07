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

	auto tile = mFlowfield->tileAtPoint(mUnit->getBody()->getPosition());
	if (tile == nullptr || tile == mFlowfield->mTargetTile) {
		end();
		return;
	}

	auto vector = mFlowfield->getVectorForTile(tile->getId());
	if (vector == nullptr) {
		end();
		return;
	}

	mUnit->getBody()->setVelocity(vector);
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