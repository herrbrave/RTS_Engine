#include"Physics.h"

BlockBody::BlockBody(float x, float y, float width, float height) : Body() {
	mPosition->set(x, y);
	mWidth = width;
	mHeight = height;
}

bool BlockBody::checkPoint(vector2f& point) {
	Extent extent(getExtent());
	return (extent.x0 <= point.x
		&& point.x <= extent.x1
		&& extent.y0 <= point.y
		&& point.y <= extent.y1);
}

bool BlockBody::checkCollision(Body& body) {
	Extent extent(getExtent());
	Extent otherExtent(body.getExtent());

	return (extent.x0 <= otherExtent.x1
		&& extent.x1 >= otherExtent.x0
		&& extent.y0 <= otherExtent.y1
		&& extent.y1 >= otherExtent.y0);
}

Extent BlockBody::getExtent() {
	float x = mPosition->x;
	float y = mPosition->y;
	float halfWidth = (mWidth / 2);
	float halfHeight = (mHeight / 2);
	return Extent{
		x - halfWidth,
		y - halfHeight,
		x + halfWidth,
		y + halfHeight
	};
}

void PhysicsSystem::registerBody(const unsigned long id, Body* body) {
	mBodies.emplace(id, std::shared_ptr<Body>(body));
}

void PhysicsSystem::deregisterBody(const unsigned long id) {
	mBodies.erase(mBodies.find(id));
}


Body* PhysicsSystem::getBody(const unsigned long id) {
	return mBodies.at(id).get();
}

void PhysicsSystem::update(Uint32 delta) {
	float step(float(delta) / float(1000));


	// TODO: add steering later.
	for (auto element : mBodies) {
		vector2f velocityCopy(*element.second->getVelocity());
		if (velocityCopy.x == 0 && velocityCopy.y == 0) {
			continue;
		}

		velocityCopy *= element.second->getSpeed();
		velocityCopy *= step;

		vector2f positionCopy(*element.second->getPosition());
		positionCopy += velocityCopy;

		element.second->setPosition(&positionCopy);
	}
}