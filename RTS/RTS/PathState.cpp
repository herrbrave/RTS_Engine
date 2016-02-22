#include"PathState.h"

PathState::PathState(Entity* unit, Flowfield* flowField) {
	mUnit = unit;
	mFlowfield = flowField;
}

void PathState::begin() {
	mStarted = true;
}

void PathState::update() {
	if (!mStarted || mEnded) {
		return;
	}

	auto component = reinterpret_cast<PhysicsComponent*>(mUnit->componentContainer->getComponentByType(PHYSICS_COMPONENT_TYPE));
	auto tile = mFlowfield->tileAtPoint(component->getPosition());
	if (tile == nullptr || tile == mFlowfield->mTargetTile) {
		end();
		return;
	}

	auto vector = mFlowfield->getVectorForTile(tile->id);
	if (vector == nullptr) {
		end();
		return;
	}

	component->setVelocity(vector);
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