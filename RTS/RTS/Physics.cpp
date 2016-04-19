#include"Physics.h"

Body::Body(float x, float y, float width, float height) {
	this->speed = 0;
	this->position.reset(new vector2f(x, y));
	this->velocity.reset(new vector2f(0, 0));
	this->width = width;
	this->height = height;
}

Body::Body(const rapidjson::Value& root) {
	this->speed = root["speed"].GetDouble();
	this->position.reset(new vector2f(root["position"]));
	this->velocity.reset(new vector2f(root["velocity"]));
	this->width = root["width"].GetDouble();
	this->height = root["height"].GetDouble();
}

Body::Body(const Body& copy) {
	this->speed = copy.speed;
	this->position.reset(new vector2f(*copy.position));
	this->velocity.reset(new vector2f(*copy.velocity));
	this->width = copy.width;
	this->height = copy.height;
	if (copy.collider != nullptr) {
		this->collider.reset(new Collider(*copy.collider));
	}
}

void Body::setSpeed(float speed) {
	this->speed = speed;
}

float Body::getSpeed() {
	return speed;
}

void Body::setPosition(vector2f* position) {
	this->position->set(position);
	if (collider != nullptr) {
		collider->position->set(position);
	}
}

const vector2f* Body::getPosition() {
	return position.get();
}

void Body::setVelocity(vector2f* vector) {
	this->velocity->set(vector);
}
const vector2f* Body::getVelocity() {
	return velocity.get();
}

void Body::setCollider(Collider* collider) {
	this->collider.reset(collider);
}

const Collider& Body::getCollider() {
	return *collider.get();
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

bool Body::checkPoint(const vector2f& point) {
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
	this->position.reset(new vector2f(x, y));
	this->width = width;
	this->height = height;
}

Collider::Collider(const Collider& copy) {
	this->position.reset(new vector2f(*copy.position));
	this->width = copy.width;
	this->height = copy.height;
}

void Collider::setOnCollisionCallback(std::function<void(const Collider&)>& callback) {
	onCollisionCallback = callback;
}

bool Collider::checkCollision(const Collider& collider) const {
	Extent extent; 
	getExtent(extent);
	Extent otherExtent;
	collider.getExtent(otherExtent);

	bool collision(extent.x0 <= otherExtent.x1
		&& extent.x1 >= otherExtent.x0
		&& extent.y0 <= otherExtent.y1
		&& extent.y1 >= otherExtent.y0);
	if (collision) {
		onCollision(collider);
		collider.onCollision(*this);
	}

	return collision;
}

void Collider::onCollision(const Collider& collider) const {
	if (onCollisionCallback == nullptr) {
		return;
	}

	onCollisionCallback(collider);
}

void Collider::getExtent(Extent& extent) const {
	float x(position->x);
	float y(position->y);
	float halfWidth(width / 2);
	float halfHeight(height / 2);
	extent.x0 = (x - halfWidth);
	extent.y0 = (y - halfHeight);
	extent.x1 = (x + halfWidth);
	extent.y1 = (y + halfHeight);
}

QuadtreeNode::QuadtreeNode(float x, float y, float width, float height) {
	mBody.reset(new Body(x, y, width, height));
	mBody->setCollider(new Collider(x, y, width, height));
}

bool QuadtreeNode::check(Body* body) {
	return mBody->collider->checkCollision(*body->collider);
}

void QuadtreeNode::add(Body* body) {
	bodiesContainer.emplace(body);
}

void QuadtreeNode::remove(Body* body) {
	bodiesContainer.erase(std::find(bodiesContainer.begin(), bodiesContainer.end(), body));
}

void QuadtreeNode::clear() {
	bodiesContainer.clear();
}

bool QuadtreeNode::contains(Body* body) {
	return (bodiesContainer.find(body) != bodiesContainer.end());
}

void QuadtreeNode::getNodeExtent(Extent& extent) {
	return mBody->collider->getExtent(extent);
}

Quadtree::Quadtree(float x, float y, float width, float height) {
	mRootNode.reset(new QuadtreeNode(x, y, width, height));
	createTree(mRootNode.get());
}

void Quadtree::createTree(QuadtreeNode* parent) {
	Extent parentExtent;
	parent->getNodeExtent(parentExtent);
	float width = (parentExtent.x1 - parentExtent.x0) / 2;
	float height = (parentExtent.y1 - parentExtent.y0) / 2;
	float widthHalf = (width / 2);
	float heightHalf = (height / 2);
	float x = (parentExtent.x0 + width);
	float y = (parentExtent.y0 + height);

	if (width < MIN_SIZE || height < MIN_SIZE) {
		parent->leaf = true;
		mLeaves.push_back(parent);
		return;
	}

	// upper left corner
	QuadtreeNode* upperLeft = new QuadtreeNode(x - widthHalf, y - heightHalf, width, height);
	parent->pushChild(upperLeft);
	createTree(upperLeft);

	// upper right
	QuadtreeNode* upperRight = new QuadtreeNode(x + widthHalf, y - heightHalf, width, height);
	parent->pushChild(upperRight);
	createTree(upperRight);

	// bottom left corner
	QuadtreeNode* bottomLeft = new QuadtreeNode(x - widthHalf, y + heightHalf, width, height);
	parent->pushChild(bottomLeft);
	createTree(bottomLeft);

	// bottom right corner
	QuadtreeNode* bottomRight = new QuadtreeNode(x + widthHalf, y + heightHalf, width, height);
	parent->pushChild(bottomRight);
	createTree(bottomRight);
}

void Quadtree::clear() {
	std::stack<QuadtreeNode*> stack;
	stack.push(mRootNode.get());
	while (stack.size() > 0) {
		QuadtreeNode* node = stack.top();
		stack.pop();
		node->clear();

		for (auto child : node->children) {
			stack.push(child);
		}
	}
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
		addHelper(body, child);
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
		removeHelper(body, child);
	}
}

void Quadtree::getCollidingBodies(Body* body, std::vector<Body*>& bodies) {
	std::vector<QuadtreeNode*> nodes;
	std::stack<QuadtreeNode*> stack;
	stack.push(mRootNode.get());

	while (!stack.empty()) {
		QuadtreeNode* node = stack.top();
		stack.pop();

		if (!node->check(body)) {
			continue;
		}
		
		if (node->leaf) {
			nodes.push_back(node);
			continue;
		}

		for (QuadtreeNode* childNode : node->children) {
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

			const Collider& collider = *otherBody->collider.get();
			if (collider.checkCollision(*body->collider.get())) {
				bodies.push_back(otherBody);
			}
		}
	}
}

void Quadtree::getNeigboringBodies(Body* body, std::vector<Body*>& bodies) {
	std::vector<QuadtreeNode*> nodes;
	std::stack<QuadtreeNode*> stack;
	stack.push(mRootNode.get());

	while (!stack.empty()) {
		QuadtreeNode* node = stack.top();
		stack.pop();

		if (!node->check(body)) {
			continue;
		}

		if (node->leaf) {
			nodes.push_back(node);
			continue;
		}

		for (QuadtreeNode* childNode : node->children) {
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

void PhysicsComponent::setPosition(vector2f* position) {
	mBody->setPosition(position);
	mPhysicsNotifier->notifyPositionSet(entityId);
}

void PhysicsComponent::setCollider(Collider* collider) {
	mBody->setCollider(collider);
	mPhysicsNotifier->notifyColliderUpdate(entityId);
}

bool PhysicsComponent::isCollidable() {
	return (mBody->collider != nullptr);
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