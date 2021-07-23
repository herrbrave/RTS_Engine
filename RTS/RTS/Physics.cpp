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
	if (root.FindMember("tag") != root.MemberEnd()) {
		this->tag = root["tag"].GetString();
	}
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
		collider->colliderShape->setPosition(position);
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

void Body::setCollider(ColliderPtr collider) {
	this->collider = std::move(collider);
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
const string& Body::getTag() {
	return this->tag;
}
void Body::setTag(const string& tag) {
	this->tag = tag;
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

Collider::Collider(ColliderShape* collider) {
	this->colliderShape = ColliderShapePtr(collider);
}

Collider::Collider(const Collider& copy) {
	if (copy.colliderShape->colliderType() == ColliderType::AABB) {
		this->colliderShape = ColliderShapePtr(GCC_NEW AABBColliderShape(*copy.colliderShape));
	}
	else if (copy.colliderShape->colliderType() == ColliderType::CIRCLE) {
		this->colliderShape = ColliderShapePtr(GCC_NEW CircleColliderShape(*copy.colliderShape));
	}
	else if (copy.colliderShape->colliderType() == ColliderType::OBB) {
		this->colliderShape = ColliderShapePtr(GCC_NEW OBBColliderShape(*copy.colliderShape));
	}
}

Collider::Collider(const rapidjson::Value& root) {
	auto colliderType = COLLIDER_TYPE_VALUES.at(string(root["colliderType"].GetString()));

	if (colliderType == ColliderType::AABB) {
		this->colliderShape = std::make_shared<AABBColliderShape>(root["colliderShape"]);
	}
	else if (colliderType == ColliderType::CIRCLE) {
		this->colliderShape = std::make_shared<CircleColliderShape>(root["colliderShape"]);
	}
	else if (colliderType == ColliderType::OBB) {
		this->colliderShape = std::make_shared<OBBColliderShape>(root["colliderShape"]);
	}
}

void Collider::setOnCollisionCallback(std::function<void(const Collider&)>& callback) {
	onCollisionCallback = callback;
}

bool Collider::checkCollision(const Collider& collider) const {
	bool collision = this->colliderShape->checkCollision(collider.colliderShape);
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

void ColliderShape::setPosition(const Vector2f& position) {
	this->position->x = position.x;
	this->position->y = position.y;
}

bool checkCollisionOBB_AABB(const OBBColliderShape& obb, const AABBColliderShape& aabb) {
	Vector2fPtr position = std::make_shared<Vector2f>(aabb.position->x, aabb.position->y);
	OBBColliderShapePtr aabbToObb = std::make_shared<OBBColliderShape>(position, aabb.width, aabb.height, 0);

	return obb.checkCollision(aabbToObb);
}

bool checkCollisionOBB_Circle(const OBBColliderShape& obb, const CircleColliderShape& circle) {

	Vector2f circlePos(circle.position);
	Vector2f obbPos(obb.position);

	Vector2f diff = circlePos - obbPos;

	float angle = -obb.angle * (M_PI / 180.0f);

	diff.x = std::cos(angle) * (diff.x) - std::sin(angle) * (diff.y);
	diff.y = std::sin(angle) * (diff.x) + std::cos(angle) * (diff.y);

	diff += obbPos;

	Vector2fPtr circlePosPtr = std::make_shared<Vector2f>(diff);
	Vector2fPtr obbPosPtr = std::make_shared<Vector2f>(obbPos);

	AABBColliderShape aabb(obbPosPtr, obb.width, obb.height);
	CircleColliderShape rotatedCircle(circlePosPtr, circle.radius);

	return checkCollisionAABB_Circle(aabb, rotatedCircle);
}

bool checkCollisionAABB_Circle(const AABBColliderShape& aabb, const CircleColliderShape& circle) {
	Vector2f halfExtent = Vector2f(aabb.width / 2.0f, aabb.height / 2.0f);
	Vector2f diff = *circle.position - *aabb.position;

	Vector2f clamped = clamp(diff, halfExtent * -1.0f, halfExtent);
	Vector2f closest = *aabb.position + clamped;
	diff = closest - circle.position;

	bool collision((int)diff.magnitude() < circle.radius);

	return collision;
}

void constructManifoldAABB_Circle(const AABBColliderShape& aabb, const CircleColliderShape& circle, Manifold* manifold) {
	Vector2f norm = *aabb.position - *circle.position;
	Vector2f closest(norm);

	float halfWidth = aabb.width / 2.0f;
	float extentX = ((aabb.position->x + halfWidth) - (aabb.position->x - halfWidth)) / 2.0f;

	float halfHeight = aabb.height / 2.0f;
	float extentY = ((aabb.position->y + halfHeight) - (aabb.position->y - halfHeight)) / 2.0f;
	closest.x = clamp(-extentX, extentX, closest.x);
	closest.y = clamp(-extentY, extentY, closest.y);

	bool inside = false;
	if (norm.x == closest.y && norm.y == closest.y) {
		inside = true;
		if (abs(norm.x) > abs(norm.y)) {
			if (closest.x > 0.0f) {
				closest.x = extentX;
			}
			else {
				closest.x = -extentX;
			}
		}
		else {
			if (closest.y > 0.0f) {
				closest.y = extentY;
			}
			else {
				closest.y = -extentY;
			}
		}
	}

	Vector2f normal = norm - closest;
	float dist = normal.magnitude();
	float rad = circle.radius;

	if (inside) {
		manifold->normal->set(normal * -1.0f);
		//manifold->penetration - rad - dist;
	}
	else {
		manifold->normal->set(normal);
		//manifold->penetration - rad - dist;
	}
}

void intersectAABBSegment(const AABBColliderShape& aabb, const Vector2f& position, const Vector2f& delta, Manifold* manifold, float paddingX, float paddingY) {
	float scaleX = 1.0f / delta.x;
	float scaleY = 1.0f / delta.y;
	float signX = scaleX > 0.0f ? 1.0 : -1.0;
	float signY = scaleY > 0.0f ? 1.0 : -1.0;
	float halfX = (float) aabb.width / 2.0f;
	float halfY = (float) aabb.height / 2.0f;
	float nearTimeX = (aabb.position->x - signX * (halfX + paddingX) - position.x) * scaleX;
	float nearTimeY = (aabb.position->y - signY * (halfY + paddingY) - position.y) * scaleY;
	float farTimeX = (aabb.position->x + signX * (halfX + paddingX) - position.x) * scaleX;
	float farTimeY = (aabb.position->y + signY * (halfY + paddingY) - position.y) * scaleY;

	if (nearTimeX > farTimeY || nearTimeY > farTimeX) {
		return;
	}

	float nearTime = nearTimeX > nearTimeY ? nearTimeX : nearTimeY;
	float farTime = farTimeX < farTimeY ? farTimeX : farTimeY;

	if (nearTime >= 1.0f || farTime <= 0.0f) {
		return;
	}

	manifold->time = clamp(nearTime, 0.0f, 1.0f);
	if (nearTimeX > nearTimeY) {
		manifold->normal->set(-signX, 0.0f);
	}
	else {
		manifold->normal->set(0.0f, -signY);
	}

	manifold->delta->set((1.0f - manifold->time) * -delta.x, (1.0f - manifold->time) * -delta.y);
	manifold->position->set(position.x + delta.x * manifold->time, position.y + delta.y * manifold->time);
}

float clamp(float value, float min, float max) {
	return std::min(max, std::max(min, value));
}

const Vector2f& clamp(const Vector2f& value, const Vector2f& min, const Vector2f& max) {
	return Vector2f(clamp(value.x, min.x, max.x), clamp(value.y, min.y, max.y));
}

void project(Vector2f& a, Vector2f& b, Vector2f& result) {
	result.set(b * (a.dot(b) / b.dot(b)));
}

bool AABBColliderShape::checkCollision(const ColliderShapePtr& collider) const {

	if (collider->colliderType() == ColliderType::AABB) {
		AABBColliderShapePtr aabb = dynamic_pointer_cast<AABBColliderShape>(collider);
		ExtentPtr extent = getExtent();
		ExtentPtr otherExtent = aabb->getExtent();

		bool collision(extent->x0 < otherExtent->x1
			&& extent->x1 > otherExtent->x0
			&& extent->y0 < otherExtent->y1
			&& extent->y1 > otherExtent->y0);

		return collision;
	}
	else if (collider->colliderType() == ColliderType::CIRCLE) {
		CircleColliderShapePtr circle = dynamic_pointer_cast<CircleColliderShape>(collider);

		return checkCollisionAABB_Circle(*this, *circle);
	}
	else if (collider->colliderType() == ColliderType::OBB) {
		OBBColliderShapePtr obb = dynamic_pointer_cast<OBBColliderShape>(collider);

		return checkCollisionOBB_AABB(*obb, *this);
	}

	return false;
}

void AABBColliderShape::intersect(const ColliderShapePtr& collider, Manifold* manifold) const {

	if (collider->colliderType() == ColliderType::AABB) {
		AABBColliderShapePtr aabb = dynamic_pointer_cast<AABBColliderShape>(collider);

		Vector2f delta = *aabb->position - *this->position;
		Vector2f halfThis((float)this->width / 2.0f, (float)this->height / 2.0f);
		Vector2f halfAABB((float)aabb->width / 2.0f, (float)aabb->height / 2.0f);
		Vector2f penetrate(halfThis.x + halfAABB.x - abs(delta.x), halfThis.y + halfAABB.y - abs(delta.y));

		if (penetrate.x <= 0 || penetrate.y <= 0) {
			return;
		}

		if (penetrate.x < penetrate.y) {
			float sx = penetrate.x < 0 ? -1.0f : 1.0f;
			manifold->delta->x = penetrate.x * sx;
			manifold->normal->x = sx;
			manifold->position->x = this->position->x + (halfThis.x * sx);
			manifold->position->y = aabb->position->y;
		}
		else {
			float sy = penetrate.y < 0 ? -1.0f : 1.0f;
			manifold->delta->y = penetrate.y * sy;
			manifold->normal->y = sy;
			manifold->position->x = aabb->position->x;
			manifold->position->y = this->position->y + (halfThis.y * sy);
		}
	}
	else if (collider->colliderType() == ColliderType::CIRCLE) {
		CircleColliderShapePtr circle = dynamic_pointer_cast<CircleColliderShape>(collider);
		constructManifoldAABB_Circle(*this, *circle, manifold);
	}
	else if (collider->colliderType() == ColliderType::OBB) {
	}
}

void AABBColliderShape::sweep(const ColliderShapePtr& collider, const Vector2f& delta, Sweep* sweep) const {
	if (collider->colliderType() == ColliderType::AABB) {
		AABBColliderShapePtr aabb = dynamic_pointer_cast<AABBColliderShape>(collider);

		if (delta.x == 0 && delta.y == 0) {
			sweep->position->set(aabb->position);
			this->intersect(collider, sweep->manifold.get());
			sweep->time = 1;
			return;
		}

		Vector2f halfThis((float)this->width / 2.0f, (float)this->height / 2.0f);
		intersectAABBSegment(*this, *aabb->position.get(), delta, sweep->manifold.get(), (float)aabb->width / 2.0f, (float)aabb->height / 2.0f);

		if (sweep->manifold->time < 1.0f) {
			sweep->time = clamp(sweep->manifold->time - EPSILON, 0.0f, 1.0f);
			sweep->position->set(aabb->position->x + delta.x * sweep->time, aabb->position->y + delta.y * sweep->time);
			Vector2f direction(delta);
			direction.normalize();
			sweep->manifold->position->set(
				clamp(sweep->manifold->position->x + direction.x * ((float)aabb->width / 2.0f), this->position->x - halfThis.x, this->position->x + halfThis.x),
				clamp(sweep->manifold->position->y + direction.y * ((float)aabb->height / 2.0f), this->position->y - halfThis.y, this->position->y + halfThis.y)
			);
		}
		else {
			sweep->position->set(*aabb->position + delta);
			sweep->time = 1.0f;
		}
	}
	else if (collider->colliderType() == ColliderType::CIRCLE) {
	}
	else if (collider->colliderType() == ColliderType::OBB) {
	}
}

ColliderType AABBColliderShape::colliderType() {
	return ColliderType::AABB;
}

ExtentPtr AABBColliderShape::getExtent() const {
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

bool CircleColliderShape::checkCollision(const ColliderShapePtr& collider) const {

	if (collider->colliderType() == ColliderType::CIRCLE) {
		CircleColliderShapePtr circle = dynamic_pointer_cast<CircleColliderShape>(collider);

		int rads = circle->radius + this->radius;
		rads *= rads;

		int dist = pow(this->position->x + circle->position->x, 2) + pow(this->position->y + circle->position->y, 2);
		bool collision(rads < dist);

		return collision;
	}
	else if (collider->colliderType() == ColliderType::AABB) {
		AABBColliderShapePtr aabb = dynamic_pointer_cast<AABBColliderShape>(collider);

		return checkCollisionAABB_Circle(*aabb, *this);
	}
	else if (collider->colliderType() == ColliderType::OBB) {
		OBBColliderShapePtr obb = dynamic_pointer_cast<OBBColliderShape>(collider);

		return checkCollisionOBB_Circle(*obb, *this);
	}

	return false;
}


void CircleColliderShape::intersect(const ColliderShapePtr& collider, Manifold* manifold) const {

	if (collider->colliderType() == ColliderType::CIRCLE) {
		CircleColliderShapePtr circle = dynamic_pointer_cast<CircleColliderShape>(collider);

		Vector2f norm = *circle->position - *this->position;

		float rads = this->radius + circle->radius;
		rads *= rads;

		float dist = norm.magnitude();

		if (dist != 0) {
			//manifold->penetration - rads - dist;
			norm *= 1.0f / dist;
			manifold->normal->set(norm);
		}
		else {
			//manifold->penetration = this->radius;
			manifold->normal->x = 1;
		}
	}
	else if (collider->colliderType() == ColliderType::AABB) {
		AABBColliderShapePtr aabb = dynamic_pointer_cast<AABBColliderShape>(collider);
		constructManifoldAABB_Circle(*aabb, *this, manifold);
	}
	else if (collider->colliderType() == ColliderType::OBB) {

	}
}

void CircleColliderShape::sweep(const ColliderShapePtr& collider, const Vector2f& delta, Sweep* sweep) const {
	if (collider->colliderType() == ColliderType::AABB) {
	}
	else if (collider->colliderType() == ColliderType::CIRCLE) {
	}
	else if (collider->colliderType() == ColliderType::OBB) {
	}
}

ColliderType CircleColliderShape::colliderType() {
	return ColliderType::CIRCLE;
}

void OBBColliderShape::setPosition(const Vector2f& position) {
	ColliderShape::setPosition(position);
}

bool OBBColliderShape::checkCollision(const ColliderShapePtr& collider) const {

	if (collider->colliderType() == ColliderType::OBB) {
		OBBColliderShapePtr obb = dynamic_pointer_cast<OBBColliderShape>(collider);
		return this->overlapOneWay(*obb) && obb->overlapOneWay(*this);
	}
	else if (collider->colliderType() == ColliderType::CIRCLE) {
		CircleColliderShapePtr circle = dynamic_pointer_cast<CircleColliderShape>(collider);

		return checkCollisionOBB_Circle(*this, *circle);
	}
	else if (collider->colliderType() == ColliderType::AABB) {
		AABBColliderShapePtr aabb = dynamic_pointer_cast<AABBColliderShape>(collider);

		return checkCollisionOBB_AABB(*this, *aabb);
	}

	return false;
}

void OBBColliderShape::intersect(const ColliderShapePtr& collider, Manifold* manifold) const {

}

void OBBColliderShape::sweep(const ColliderShapePtr& collider, const Vector2f& delta, Sweep* sweep) const {
	if (collider->colliderType() == ColliderType::AABB) {
	}
	else if (collider->colliderType() == ColliderType::CIRCLE) {
	}
	else if (collider->colliderType() == ColliderType::OBB) {
	}
}

bool OBBColliderShape::overlapOneWay(const OBBColliderShape& obb) const {

	for (int index = 0; index < 2; index++) {
		float t = obb.corners[0]->dot(*this->axis[index]);

		float tMin = t;
		float tMax = t;

		for (int corner = 1; corner < 4; corner++) {
			t = obb.corners[corner]->dot(*this->axis[index]);

			if (t < tMin) {
				tMin = t;
			}
			else if (t > tMax) {
				tMax = t;
			}
		}

		if ((tMin > 1 + this->origin[index]) || (tMax < this->origin[index])) {
			return false;
		}
	}

	return true;
}

ColliderType OBBColliderShape::colliderType() {
	return ColliderType::OBB;
}

void OBBColliderShape::setAngle(int angle) {
	this->angle = angle;

	this->setup();
}

void OBBColliderShape::setup() {
	float rads = (this->angle / 180.0f) * M_PI;
	Vector2f x(std::cos(rads), std::sin(rads));
	Vector2f y(-std::sin(rads), std::cos(rads));
	x *= this->width / 2.0f;
	y *= this->height / 2.0f;

	this->corners[0]->set(*this->position - x - y);
	this->corners[1]->set(*this->position + x - y);
	this->corners[2]->set(*this->position + x + y);
	this->corners[3]->set(*this->position - x + y);

	axis[0]->set(*corners[1] - *corners[0]);
	axis[1]->set(*corners[3] - *corners[0]);

	for (int index = 0; index < 2; index++) {
		*axis[index] *= (1.0f / std::pow(axis[index]->magnitude(), 2));
		origin[index] = corners[0]->dot(*axis[index]);
	}
}

QuadtreeNode::QuadtreeNode(float x, float y, float width, float height) {
	mBody.reset(GCC_NEW Body(-1, x, y, width, height));
	mBody->setCollider(ColliderPtr(GCC_NEW Collider(GCC_NEW AABBColliderShape(std::make_shared<Vector2f>(x, y), (int) width, (int) height))));
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
	return static_cast<AABBColliderShape>(*mBody->collider->colliderShape).getExtent();
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

void PhysicsComponent::setCollider(ColliderPtr collider) {
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

const string& PhysicsComponent::getTag() {
	return mBody->getTag();
}
void PhysicsComponent::setTag(const string& tag) {
	this->mBody->setTag(tag);
}

bool BasicBehavior::updateBehavior(float step, BodyPtr& body, QuadtreePtr quadtree) {
	Vector2f delta(body->getVelocity());

	if (delta.x == 0 && delta.y == 0 && !body->hasTarget()) {
		return false;
	}

	Vector2f position(body->getPosition());
	if (body->hasTarget()) {
		TargetPtr target = makeShared(body->getTarget());
		
		Vector2f targetPosition(target->getTargetPosition());

		Vector2f velocityAtTarget = targetPosition - position;
		float distance = velocityAtTarget.magnitude();
		velocityAtTarget.normalize();

		Vector2f velocityDiff = velocityAtTarget - delta;
		if (velocityDiff.magnitude() > 0.1f) {
			delta.set(velocityAtTarget);
		}
		
		delta *= body->getSpeed();
		delta *= step;

		delta.truncate(distance);
	}
	else {
		delta *= body->getSpeed() * step;
	}

	Vector2f newPosition;
	this->handleCollision(delta, newPosition, body, quadtree);

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

void BasicBehavior::handleCollision(const Vector2f& delta, Vector2f& pos, BodyPtr& body, QuadtreePtr quadtree) {

	Vector2f position(body->getPosition());

	// Handle overlap
	vector<WeakBodyPtr> bodies;

	// project body to potential new position
	*body->position += delta;
	*body->collider->colliderShape->position += delta;
	quadtree->getCollidingBodies(body, bodies);

	// remove position update so we can sweep the body for collisions.
	*body->position -= delta;
	*body->collider->colliderShape->position -= delta;
	Vector2f newPosition(position + delta);

	Sweep sweep;
	sweep.time = 1.0f;
	sweep.position->set(newPosition);
	for (auto bod : bodies) {
		auto bodPtr = makeShared<Body>(bod);
		Sweep currentSweep;
		bodPtr->collider->colliderShape->sweep(body->collider->colliderShape, delta, &currentSweep);
		if (currentSweep.time < sweep.time) {
			sweep = currentSweep;
		}
	}

	Vector2f finalPos = *sweep.position;
	if (sweep.time < 1.0f) {
		Vector2f velocity(body->velocity);
		Vector2f normal(sweep.manifold->normal);
		normal.x = abs(normal.x);
		normal.y = abs(normal.y);
		Vector2f update;
		project(velocity, normal, update);
		velocity -= update;
		velocity.normalize();
		body->setVelocity(velocity);
	}

	body->setPosition(finalPos);
	pos.set(finalPos);
	DEBUG_LOG("==============START================");

	DEBUG_LOG("Sweep Position: " + std::to_string(sweep.position->x) + ", " + std::to_string(sweep.position->y));
	DEBUG_LOG("Final Position: " + std::to_string(finalPos.x) + ", " + std::to_string(finalPos.y));
	DEBUG_LOG("Delta: " + std::to_string(sweep.manifold->delta->x) + ", " + std::to_string(sweep.manifold->delta->y));
	DEBUG_LOG("Velocity: " + std::to_string(body->velocity->x) + ", " + std::to_string(body->velocity->y));
	DEBUG_LOG("Time: " + std::to_string(sweep.time));

	DEBUG_LOG("===============END-================");
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