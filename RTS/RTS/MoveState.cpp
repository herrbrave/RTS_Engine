#include"MoveState.h"

void MoveState::begin() {
	mHasBegun = true;
}

void MoveState::update() {
	vector2f unitLocation = mEntity->getLocation();
	vector2f targetLocation = mTarget->getLocation();
	vector2f delta = targetLocation - unitLocation;

	float dist = delta.magnitude();
	if (dist == 0 || dist <= mSpeed) {
		mEntity->setLocation(targetLocation);
		end();
		return;
	}

	delta.normalize();
	delta = delta * float(10);

	mEntity->setLocation(delta);
}

void MoveState::end() {
	mDone = true;
}

bool MoveState::hasBegun() {
	return mHasBegun;
}
bool MoveState::isEnded() {
	return mDone;
}

MoveState::MoveState(std::shared_ptr<Entity> entity, std::shared_ptr<Entity> target) {
	mEntity = entity;
	mTarget = target;
	mSpeed = 2;
}