#include"Physics.h"

float Body::getWidth() {
	return mWidth;
}
float Body::getHeight() {
	return mHeight;
}

void Body::setWidth(float width) {
	mWidth = width;
}
void Body::setHeight(float height) {
	mHeight = height;
}

BlockBody::BlockBody(float x, float y, float width, float height) : Body(x, y, width, height) {}

bool BlockBody::checkPoint(const vector2f& point) {
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

void BlockBody::onSerialization(Serializer& serializer) const {
	serializer.writer.String("BodyType");
	serializer.writer.String("BlockBody");
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

void PhysicsComponent::setPosition(vector2f* position) {
	mBody->setPosition(position);
}

const vector2f* PhysicsComponent::getPosition() {
	return mBody->getPosition();
}

void PhysicsComponent::setVelocity(vector2f* velocity) {
	mBody->setVelocity(velocity);
}

const vector2f* PhysicsComponent::getVelocity()  {
	return mBody->getVelocity();
}

void PhysicsComponent::setSpeed(float speed) {
	mBody->setSpeed(speed);
}

float PhysicsComponent::getSpeed() {
	return mBody->getSpeed();
}

void PhysicsComponent::setSize(float width, float height) {
	mBody->setWidth(width);
	mBody->setHeight(height);
}

float PhysicsComponent::getWidth() {
	return mBody->getWidth();
}

float PhysicsComponent::getHeight() {
	return mBody->getHeight();
}