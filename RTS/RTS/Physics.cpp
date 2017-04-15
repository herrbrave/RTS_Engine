#include"Physics.h"

Body::Body(int id, float x, float y, float width, float height) {
	this->id = id;
	this->speed = 0;
	this->mass = 3;
	this->position.reset(GCC_NEW Vector2f(x, y));
	this->velocity.reset(GCC_NEW Vector2f(0, 0));
	this->width = width;
	this->height = height;
}

Body::Body(const rapidjson::Value& root) {
	this->speed = root["speed"].GetDouble();
	this->mass = root["mass"].GetDouble();
	this->position.reset(GCC_NEW Vector2f(root["position"]));
	this->velocity.reset(GCC_NEW Vector2f(root["velocity"]));
	this->width = root["width"].GetDouble();
	this->height = root["height"].GetDouble();
}

Body::Body(const Body& copy) {
	this->speed = copy.speed;
	this->mass = copy.mass;
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

void Body::setMass(float mass) {
	this->mass = mass;
}

float Body::getMass() {
	return this->mass;
}

void Body::setPosition(const Vector2f& position) {
	this->position->set(position);
	if (collider != nullptr) {
		collider->position->set(position);
	}
}

const Vector2f& Body::getPosition() {
	return *position.get();
}

void Body::setVelocity(const Vector2f& vector) {
	this->velocity->set(vector);
}

const Vector2f& Body::getVelocity() {
	return *velocity.get();
}

void Body::setCollider(Collider* collider) {
	this->collider = ColliderPtr(collider);
}

WeakColliderPtr Body::getCollider() {
	return WeakColliderPtr(collider);
}

bool Body::isCollidable() {
	return (collider != nullptr);
}

void Body::setTarget(TargetPtr target) {
	this->target = target;
}

WeakTargetPtr Body::getTarget() {
	return WeakTargetPtr(this->target);
}

bool Body::hasTarget() {
	return (target != nullptr);
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

void Collider::setOnCollisionCallback(std::function<void(const Collider&)>& callback) {
	onCollisionCallback = callback;
}

bool Collider::checkCollision(const Collider& collider) const {
	ExtentPtr extent = getExtent();
	ExtentPtr otherExtent = collider.getExtent();

	bool collision(extent->x0 <= otherExtent->x1
		&& extent->x1 >= otherExtent->x0
		&& extent->y0 <= otherExtent->y1
		&& extent->y1 >= otherExtent->y0);
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
	mBody.reset(GCC_NEW Body(-1, x, y, width, height));
	mBody->setCollider(GCC_NEW Collider(x, y, width, height));
}

bool QuadtreeNode::check(BodyPtr body) {
	return mBody->collider->checkCollision(*body->collider);
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

			if (otherBody->collider->checkCollision(*body->collider)) {
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

BodyTarget::BodyTarget(BodyPtr body) {
	this->mBody = body;
}

const Vector2f& BodyTarget::getTargetPosition() {
	return this->mBody->getPosition();
}

PositionTarget::PositionTarget(Vector2fPtr position) {
	this->mPosition = position;
}

const Vector2f& PositionTarget::getTargetPosition() {
	return this->mPosition;
}

void PhysicsComponent::setPosition(const Vector2f& position) {
	mBody->setPosition(position);

	EntityPositionSetEventData* eventData = GCC_NEW EntityPositionSetEventData(entityId, position, SDL_GetTicks());
	EventManager::getInstance().pushEvent(eventData);
}

void PhysicsComponent::setCollider(Collider* collider) {
	mBody->setCollider(collider);

	EntityCollisionSetEventData* eventData = GCC_NEW EntityCollisionSetEventData(entityId, SDL_GetTicks());
	EventManager::getInstance().pushEvent(eventData);
}

bool PhysicsComponent::isCollidable() {
	return (mBody->collider != nullptr);
}

const Vector2f& PhysicsComponent::getPosition() {
	return mBody->getPosition();
}

void PhysicsComponent::setVelocity(const Vector2f& velocity) {
	mBody->setVelocity(velocity);
}

const Vector2f& PhysicsComponent::getVelocity()  {
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

bool BasicBehavior::updateBehavior(float step, BodyPtr& body, QuadtreePtr quadtree) {
	Vector2f velocity(body->getVelocity());

	if (velocity.x == 0 && velocity.y == 0 && !body->hasTarget()) {
		return false;
	}

	Vector2f position(body->getPosition());
	if (body->hasTarget()) {
		TargetPtr target = makeShared(body->getTarget());
		
		Vector2f targetPosition(target->getTargetPosition());

		Vector2f velocityAtTarget = targetPosition - position;
		float distance = velocityAtTarget.magnitude();
		velocityAtTarget.normalize();

		Vector2f velocityDiff = velocityAtTarget - velocity;
		if (velocityDiff.magnitude() > 0.1f) {
			velocity.set(velocityAtTarget);
		}
		
		velocity *= body->getSpeed();
		velocity *= step;

		velocity.truncate(distance);
	}
	else {
		velocity *= body->getSpeed();
		velocity *= step;
	}

	Vector2f newPosition(position + velocity);

 	body->setPosition(newPosition);

	velocity.normalize();
	body->setVelocity(velocity);

	if (body->hasTarget()) {
		TargetPtr target = makeShared(body->getTarget());

		Vector2f targetPosition = target->getTargetPosition();

		Vector2f distanceVector = targetPosition - newPosition;
		if (distanceVector.magnitude() < target->getThreshold()) {
			body->setTarget(nullptr);
			body->setVelocity(ZERO_VECTOR);
		}
	}

	return true;
}

bool SteeringBehavior::updateBehavior(float step, BodyPtr& body, QuadtreePtr quadtree) {

	Vector2f velocity(body->getVelocity());

	if (velocity.x == 0 && velocity.y == 0 && !body->hasTarget()) {
		return false;
	}

	Vector2f position(body->getPosition());

	if (body->hasTarget()) {
		TargetPtr target = makeShared(body->getTarget());
		Vector2f targetPosition(target->getTargetPosition());

		Vector2f velocityAtTarget = targetPosition - position;
		float distance = velocityAtTarget.magnitude();
		velocityAtTarget.normalize();

		Vector2f steering = velocityAtTarget - velocity;
		// TODO: Set a max force somewhere.
		steering *= float(1 / body->getMass());

		Vector2f velocityDiff = velocityAtTarget - velocity;
		if (steering.magnitude() > 0.1f) {
			velocity += steering;
			velocity.normalize();
		}
		else if (velocityDiff.magnitude() > 0.1f) {
			velocity.set(velocityAtTarget);
		}

		velocity *= body->getSpeed();
		velocity *= step;

		velocity.truncate(distance);
	}
	else {
		velocity *= body->getSpeed();
		velocity *= step;
	}

	Vector2f newPosition(position + velocity);

	body->setPosition(newPosition);

	velocity.normalize();
	body->setVelocity(velocity);

	if (body->hasTarget()) {
		TargetPtr target = makeShared(body->getTarget());

		Vector2f targetPosition = target->getTargetPosition();

		Vector2f distanceVector = targetPosition - newPosition;
		if (distanceVector.magnitude() < target->getThreshold()) {
			body->setTarget(nullptr);
			body->setVelocity(ZERO_VECTOR);
		}
	}

	return true;
}

Vector2f SteeringBehavior::applySteeringToVelocity(BodyPtr& body, float step) {

	Vector2f velocity = body->getVelocity();
	velocity *= body->getSpeed();

	TargetPtr target = makeShared(body->getTarget());
	Vector2f targetPosition = target->getTargetPosition();

	Vector2f position = body->getPosition();
	Vector2f desiredVelocity = targetPosition - position;
	desiredVelocity.truncate(body->getSpeed());

	Vector2f steering = desiredVelocity - velocity;
	// TODO: Set a max force somewhere.
	steering.truncate(body->getSpeed() / 2.0f);
	steering *= float(1 / body->getMass());

	velocity += steering;
	velocity.normalize();

	// figure out what position we're at to see if we're within the threshold.
	Vector2f calculatedVelocity(velocity * body->getSpeed());
	calculatedVelocity *= step;

	Vector2f newPosition(position + calculatedVelocity);

	Vector2f distanceVector = targetPosition - newPosition;
	if (distanceVector.magnitude() < target->getThreshold()) {
		body->setTarget(nullptr);
		body->setVelocity(Vector2f());
	}

	return velocity;
}