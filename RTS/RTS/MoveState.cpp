#include"MoveState.h"

void MoveState::begin() {
	mHasBegun = true;
}

void MoveState::update() {
	auto unitComponent = reinterpret_cast<PhysicsComponent*>(mEntity->componentContainer->getComponentByType(ComponentType::PHYSICS_COMPONENT));
	auto targetComponent = reinterpret_cast<PhysicsComponent*>(mTarget->componentContainer->getComponentByType(ComponentType::PHYSICS_COMPONENT));

	vector2f unitLocation = *unitComponent->getPosition();
	vector2f targetLocation = *targetComponent->getPosition();
	vector2f delta = targetLocation - unitLocation;

	float dist = delta.magnitude();
	if (dist == 0 || dist <= mSpeed) {
		unitComponent->setPosition(&targetLocation);
		end();
		return;
	}

	delta.normalize();

	unitComponent->setVelocity(&delta);
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