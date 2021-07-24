#include"PathState.h"

PathState::PathState(EntityPtr unit, FlowfieldPtr flowField) {
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

	auto component = makeShared(mUnit->getComponentByType<PhysicsComponent>(ComponentType::PHYSICS_COMPONENT));
	if (mFlowfield->tileExistsAtPoint(component->getPosition()) || mFlowfield->tileAtPoint(component->getPosition())->id == mFlowfield->mTargetTile->id) {
		end();
		return;
	}

	auto tile = mFlowfield->tileAtPoint(component->getPosition());
	auto vector = mFlowfield->getVectorForTile(tile->id);
	if (vector.x == 0 && vector.y == 0) {
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