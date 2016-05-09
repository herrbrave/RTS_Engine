#include"Physics.h"

Body::Body(float x, float y, float width, float height) {
	this->speed = 0;
	this->position.reset(GCC_NEW Vector2f(x, y));
	this->velocity.reset(GCC_NEW Vector2f(0, 0));
	this->width = width;
	this->height = height;
}

Body::Body(const rapidjson::Value& root) {
	this->speed = root["speed"].GetDouble();
	this->position.reset(GCC_NEW Vector2f(root["position"]));
	this->velocity.reset(GCC_NEW Vector2f(root["velocity"]));
	this->width = root["width"].GetDouble();
	this->height = root["height"].GetDouble();
}

Body::Body(const Body& copy) {
	this->speed = copy.speed;
	this->position.reset(GCC_NEW Vector2f(*copy.position));
	this->velocity.reset(GCC_NEW Vector2f(*copy.velocity));
	this->width = copy.width;
	this->height = copy.height;
	if (copy.collider != nullptr) {
		this->collider.reset(GCC_NEW Collider(*copy.collider));
	}
}

void Body::setSpeed(float speed) {
	this->speed = speed;
}

float Body::getSpeed() {
	return speed;
}

void Body::setPosition(Vector2fPtr position) {
	this->position->set(position.get());
	if (collider != nullptr) {
		collider->position->set(position.get());
	}
}

WeakVector2fPtr Body::getPosition() {
	return WeakVector2fPtr(position);
}

void Body::setVelocity(Vector2fPtr vector) {
	this->velocity->set(position.get());
}

WeakVector2fPtr Body::getVelocity() {
	return WeakVector2fPtr(velocity);
}

void Body::setCollider(ColliderPtr collider) {
	this->collider = collider;
}

WeakColliderPtr Body::getCollider() {
	return WeakColliderPtr(collider);
}

bool Body::isCollidable() {
	return (collider != nullptr);
}

float Body::getWidth() {
	return width;
}
float Body::getHeight() {
	return height;
}

void Body::setWidth(float width) {
	this->width = width;
}
void Body::setHeight(float height) {
	this->height = height;
}

bool Body::checkPoint(const Vector2f& point) {
	Extent extent;

	float width(width / 2);
	float height(height / 2);
	extent.x0 = (position->x - width);
	extent.y0 = (position->y - height);
	extent.x1 = (position->x + width);
	extent.y1 = (position->y + height);

	return (extent.x0 <= point.x
		&& point.x <= extent.x1
		&& extent.y0 <= point.y
		&& point.y <= extent.y1);
}

Collider::Collider(float x, float y, float width, float height) {
	this->position = Vector2fPtr(GCC_NEW Vector2f(x, y));
	this->width = width;
	this->height = height;
}

Collider::Collider(const Collider& copy) {
	this->position = Vector2fPtr(GCC_NEW Vector2f(*copy.position));
	this->width = copy.width;
	this->height = copy.height;
}

void Collider::setOnCollisionCallback(std::function<void(ColliderPtr)>& callback) {
	onCollisionCallback = callback;
}

bool Collider::checkCollision(ColliderPtr collider) const {
	ExtentPtr extent = getExtent();
	ExtentPtr otherExtent = collider->getExtent();

	bool collision(extent->x0 <= otherExtent->x1
		&& extent->x1 >= otherExtent->x0
		&& extent->y0 <= otherExtent->y1
		&& extent->y1 >= otherExtent->y0);
	if (collision) {
		onCollision(collider);
		collider->onCollision(ColliderPtr(const_cast<Collider*>(this)));
	}

	return collision;
}

void Collider::onCollision(ColliderPtr collider) const {
	if (onCollisionCallback == nullptr) {
		return;
	}

	onCollisionCallback(collider);
}

ExtentPtr Collider::getExtent() const {
	float x(position->x);
	float y(position->y);
	float halfWidth(width / 2);
	float halfHeight(height / 2); 
	ExtentPtr extent(GCC_NEW Extent());
	extent->x0 = (x - halfWidth);
	extent->y0 = (y - halfHeight);
	extent->x1 = (x + halfWidth);
	extent->y1 = (y + halfHeight);

	return extent;
}

QuadtreeNode::QuadtreeNode(float x, float y, float width, float height) {
	mBody.reset(GCC_NEW Body(x, y, width, height));
	mBody->setCollider(ColliderPtr(GCC_NEW Collider(x, y, width, height)));
}

bool QuadtreeNode::check(BodyPtr body) {
	return mBody->collider->checkCollision(body->collider);
}

void QuadtreeNode::add(BodyPtr body) {
	bodiesContainer.emplace(body);
}

void QuadtreeNode::remove(BodyPtr body) {
	bodiesContainer.erase(std::find(bodiesContainer.begin(), bodiesContainer.end(), body));
}

void QuadtreeNode::clear() {
	bodiesContainer.clear();
}

bool QuadtreeNode::contains(BodyPtr body) {
	return (bodiesContainer.find(body) != bodiesContainer.end());
}

ExtentPtr QuadtreeNode::getNodeExtent() {
	return mBody->collider->getExtent();
}

Quadtree::Quadtree(float x, float y, float width, float height) {
	mRootNode.reset(new QuadtreeNode(x, y, width, height));
	createTree(mRootNode);
}

void Quadtree::createTree(QuadtreeNodePtr parent) {
	ExtentPtr parentExtent = parent->getNodeExtent();
	float width = (parentExtent->x1 - parentExtent->x0) / 2;
	float height = (parentExtent->y1 - parentExtent->y0) / 2;
	float widthHalf = (width / 2);
	float heightHalf = (height / 2);
	float x = (parentExtent->x0 + width);
	float y = (parentExtent->y0 + height);

	if (width < MIN_SIZE || height < MIN_SIZE) {
		parent->leaf = true;
		mLeaves.push_back(parent);
		return;
	}

	// upper left corner
	QuadtreeNodePtr upperLeft(GCC_NEW QuadtreeNode(x - widthHalf, y - heightHalf, width, height));
	parent->pushChild(upperLeft);
	createTree(upperLeft);

	// upper right
	QuadtreeNodePtr upperRight(GCC_NEW QuadtreeNode(x + widthHalf, y - heightHalf, width, height));
	parent->pushChild(upperRight);
	createTree(upperRight);

	// bottom left corner
	QuadtreeNodePtr bottomLeft(GCC_NEW QuadtreeNode(x - widthHalf, y + heightHalf, width, height));
	parent->pushChild(bottomLeft);
	createTree(bottomLeft);

	// bottom right corner
	QuadtreeNodePtr bottomRight(GCC_NEW QuadtreeNode(x + widthHalf, y + heightHalf, width, height));
	parent->pushChild(bottomRight);
	createTree(bottomRight);
}

void Quadtree::clear() {
	std::stack<QuadtreeNodePtr> stack;
	stack.push(mRootNode);
	while (stack.size() > 0) {
		QuadtreeNodePtr node = stack.top();
		stack.pop();
		node->clear();

		for (auto child : node->children) {
			stack.push(child);
		}
	}
}

void Quadtree::addBody(BodyPtr body) {
	addHelper(body, mRootNode);
}

void  Quadtree::addHelper(BodyPtr body, QuadtreeNodePtr node) {
	if (!node->check(body)) {
		return;
	}

	node->add(body);
	if (node->leaf) {
		return;
	}

	for (auto child : node->children) {
		addHelper(body, child);
	}
}

void Quadtree::removeBody(BodyPtr body) {
	removeHelper(body, mRootNode);
}

void Quadtree::removeHelper(BodyPtr body, QuadtreeNodePtr node) {
	if (!node->contains(body)) {
		return;
	}

	node->remove(body);
	for (auto child : node->children) {
		removeHelper(body, child);
	}
}

void Quadtree::getCollidingBodies(BodyPtr body, std::vector<WeakBodyPtr>& bodies) {
	std::vector<QuadtreeNodePtr> nodes;
	std::stack<QuadtreeNodePtr> stack;
	stack.push(mRootNode);

	while (!stack.empty()) {
		QuadtreeNodePtr node = stack.top();
		stack.pop();

		if (!node->check(body)) {
			continue;
		}
		
		if (node->leaf) {
			nodes.push_back(node);
			continue;
		}

		for (QuadtreeNodePtr childNode : node->children) {
			stack.push(childNode);
		}
	}

	for (auto leaf : nodes) {

		// check against all bodies in the leaf
		for (auto otherBody : leaf->bodiesContainer) {
			// make sure we don't collide with ourselves.
			if (otherBody == body) {
				continue;
			}

			if (otherBody->collider->checkCollision(body->collider)) {
				bodies.push_back(otherBody);
			}
		}
	}
}

void Quadtree::getNeigboringBodies(BodyPtr body, std::vector<BodyPtr>& bodies) {
	std::vector<QuadtreeNodePtr> nodes;
	std::stack<QuadtreeNodePtr> stack;
	stack.push(mRootNode);

	while (!stack.empty()) {
		QuadtreeNodePtr node = stack.top();
		stack.pop();

		if (!node->check(body)) {
			continue;
		}

		if (node->leaf) {
			nodes.push_back(node);
			continue;
		}

		for (QuadtreeNodePtr childNode : node->children) {
			stack.push(childNode);
		}
	}

	for (auto leaf : nodes) {

		// check against all bodies in the leaf
		for (auto otherBody : leaf->bodiesContainer) {
			// make sure we don't collide with ourselves.
			if (otherBody == body) {
				continue;
			}

			if (std::find(bodies.begin(), bodies.end(), otherBody) == bodies.end()) {
				bodies.push_back(otherBody);
			}
		}
	}
}

void PhysicsComponent::setPosition(Vector2fPtr position) {
	mBody->setPosition(position);
	mPhysicsNotifier->notifyPositionSet(entityId);
}

void PhysicsComponent::setCollider(ColliderPtr collider) {
	mBody->setCollider(collider);
	mPhysicsNotifier->notifyColliderUpdate(entityId);
}

bool PhysicsComponent::isCollidable() {
	return (mBody->collider != nullptr);
}

WeakVector2fPtr PhysicsComponent::getPosition() {
	return mBody->getPosition();
}

void PhysicsComponent::setVelocity(Vector2fPtr velocity) {
	mBody->setVelocity(velocity);
}

WeakVector2fPtr PhysicsComponent::getVelocity()  {
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