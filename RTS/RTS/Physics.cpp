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

QuadtreeNode::QuadtreeNode(float x, float y, float width, float height) {
	mBody.reset(new BlockBody(x, y, width, height));
}

bool QuadtreeNode::check(Body* body) {
	return mBody->checkCollision(*body);
}

void QuadtreeNode::add(Body* body) {
	bodiesContainer.emplace(body);
}

void QuadtreeNode::remove(Body* body) {
	bodiesContainer.erase(std::find(bodiesContainer.begin(), bodiesContainer.end(), body));
}

bool QuadtreeNode::contains(Body* body) {
	return (bodiesContainer.find(body) != bodiesContainer.end());
}

Extent QuadtreeNode::getNodeExtent() {
	return mBody->getExtent();
}

Quadtree::Quadtree(float x, float y, float width, float height) {
	mRootNode.reset(new QuadtreeNode(x, y, width, height));
	createTree(mRootNode.get());
}

void Quadtree::createTree(QuadtreeNode* parent) {
	Extent parentExtent = parent->getNodeExtent();
	int width = (parentExtent.x1 - parentExtent.x0) / 2;
	int height = (parentExtent.y1 - parentExtent.y0) / 2;
	int widthHalf = (width / 2);
	int heightHalf = (height / 2);

	if (width < MIN_SIZE || height < MIN_SIZE) {
		parent->leaf = true;
		mLeaves.push_back(parent);
		return;
	}

	// upper left corner
	QuadtreeNode* upperLeft = new QuadtreeNode(parentExtent.x0 + widthHalf, parentExtent.y0 + heightHalf, width, height);
	parent->pushChild(upperLeft);
	createTree(upperLeft);

	// upper right
	QuadtreeNode* upperRight = new QuadtreeNode(parentExtent.x0 + width + widthHalf, parentExtent.y0 + heightHalf, width, height);
	parent->pushChild(upperRight);
	createTree(upperRight);

	// bottom left corner
	QuadtreeNode* bottomLeft = new QuadtreeNode(parentExtent.x0 + widthHalf, parentExtent.y0 + height + heightHalf, width, height);
	parent->pushChild(bottomLeft);
	createTree(bottomLeft);

	// bottom right corner
	QuadtreeNode* bottomRight = new QuadtreeNode(parentExtent.x0 + width + widthHalf, parentExtent.y0 + height + heightHalf, width, height);
	parent->pushChild(bottomRight);
	createTree(bottomRight);
}

void Quadtree::addBody(Body* body) {
	addHelper(body, mRootNode.get());
}

void  Quadtree::addHelper(Body* body, QuadtreeNode* node) {
	if (!node->check(body)) {
		return;
	}

	node->add(body);
	if (node->leaf) {
		return;
	}

	for (auto child : node->children) {
		addHelper(body, child.get());
	}
}

void Quadtree::removeBody(Body* body) {
	removeHelper(body, mRootNode.get());
}

void Quadtree::removeHelper(Body* body, QuadtreeNode* node) {
	if (!node->contains(body)) {
		return;
	}

	node->remove(body);
	for (auto child : node->children) {
		removeHelper(body, child.get());
	}
}

void Quadtree::getCollidingBodies(Body* body, std::vector<Body*>& bodies) {
	for (auto leaf : mLeaves) {

		// if the body is in the given leaf
		if (!leaf->contains(body)) {
			continue;
		}

		// check against all bodies in the leaf
		for (auto otherBody : leaf->bodiesContainer) {
			if (otherBody->checkCollision(*body)) {
				bodies.push_back(otherBody);
			}
		}
	}
}