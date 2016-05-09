#include"MoveState.h"

void MoveState::begin() {
	mHasBegun = true;
}

void MoveState::update() {
	auto unitComponent = makeShared(mEntity->getComponentByType<PhysicsComponent>(ComponentType::PHYSICS_COMPONENT));
	auto targetComponent = makeShared(mTarget->getComponentByType<PhysicsComponent>(ComponentType::PHYSICS_COMPONENT));

	Vector2fPtr unitLocation = makeShared(unitComponent->getPosition());
	Vector2fPtr targetLocation = makeShared(targetComponent->getPosition());
	Vector2f delta = *targetLocation - *unitLocation;

	float dist = delta.magnitude();
	if (dist == 0 || dist <= mSpeed) {
		unitComponent->setPosition(targetLocation);
		end();
		return;
	}

	delta.normalize();

	unitComponent->setVelocity(Vector2fPtr(&delta));
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