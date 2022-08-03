#ifndef __PHYSICS_H__
#define __PHYSICS_H__

#include"Component.h"
#include"Event.h"
#include"Vector2f.h"

#include<functional>
#include<map>
#include<SDL.h>
#include<stack>
#include<unordered_map>
#include<unordered_set>


enum ColliderType : Uint8 {
	AABB = 0,
	CIRCLE = 1,
	OBB = 2,
	RAY = 3,
};
const unordered_map<string, ColliderType> COLLIDER_TYPE_VALUES {
	{ "AABB", ColliderType::AABB },
	{ "CIRCLE", ColliderType::CIRCLE },
	{ "OBB", ColliderType::OBB }
};

struct Extent {
	float x0, y0, x1, y1;
};
typedef shared_ptr<Extent> ExtentPtr;
typedef weak_ptr<Extent> WeakExtentPtr;

class Collider;
typedef shared_ptr<Collider> ColliderPtr;
typedef weak_ptr<Collider> WeakColliderPtr;

class Manifold;
typedef shared_ptr<Manifold> ManifoldPtr;
typedef weak_ptr<Manifold> WeakManifoldPtr;

class Sweep;
typedef shared_ptr<Sweep> SweepPtr;
typedef weak_ptr<Sweep> WeakSweepPtr;

class ColliderShape;
typedef shared_ptr<ColliderShape> ColliderShapePtr;
typedef weak_ptr<ColliderShape> WeakColliderShapePtr;

class AABBColliderShape;
typedef shared_ptr<AABBColliderShape> AABBColliderShapePtr;
typedef weak_ptr<AABBColliderShape> WeakAABBColliderShapePtr;

class CircleColliderShape;
typedef shared_ptr<CircleColliderShape> CircleColliderShapePtr;
typedef weak_ptr<CircleColliderShape> WeakCircleColliderShapePtr;

class OBBColliderShape;
typedef shared_ptr<OBBColliderShape> OBBColliderShapePtr;
typedef weak_ptr<OBBColliderShape> WeakOBBColliderShapePtr;

class RayColliderShape;
typedef shared_ptr<RayColliderShape> RayColliderShapePtr;
typedef weak_ptr<RayColliderShape> WeakRayColliderShapePtr;

const static float EPSILON = 0.000001f;

bool checkCollisionOBB_AABB(const OBBColliderShape& obb, const AABBColliderShape& aabb);
bool checkCollisionOBB_Circle(const OBBColliderShape& obb, const CircleColliderShape& circle);
bool checkCollisionAABB_Circle(const AABBColliderShape& aabb, const CircleColliderShape& circle);
bool checkCollisionAABB_Ray(const AABBColliderShape& aabb, const RayColliderShape& ray);
bool checkCollisionCircle_Ray(const RayColliderShape& obb, const CircleColliderShape& circle);
void constructManifoldAABB_Circle(const AABBColliderShape& aabb, const CircleColliderShape& circle, Manifold* manifold);
void intersectAABBSegment(const AABBColliderShape& aabb, const Vector2f& position, const Vector2f& delta, Manifold* manifold, float paddingX = 0.0f, float paddingY = 0.0f);

float clamp(float value, float min, float max);
const Vector2f& clamp(const Vector2f& value, const Vector2f& min, const Vector2f& max);
void project(Vector2f& a, Vector2f& b, Vector2f& result);

class Body;
typedef shared_ptr<Body> BodyPtr;
typedef weak_ptr<Body> WeakBodyPtr;
typedef vector<Body*> BodyList;

class QuadtreeNode;
typedef shared_ptr<QuadtreeNode> QuadtreeNodePtr;
typedef weak_ptr<QuadtreeNode> WeakQuadtreeNodePtr;

class Quadtree;
typedef shared_ptr<Quadtree> QuadtreePtr;
typedef weak_ptr<Quadtree> WeakQuadtreePtr;

class Target;
typedef shared_ptr<Target> TargetPtr;
typedef weak_ptr<Target> WeakTargetPtr;

class PhysicsComponent;
typedef shared_ptr<PhysicsComponent> PhysicsComponentPtr;
typedef weak_ptr<PhysicsComponent> WeakPhysicsComponentPtr;

class PhysicsBehavior;
typedef shared_ptr<PhysicsBehavior> PhysicsBehaviorPtr;
typedef weak_ptr<PhysicsBehavior> WeakPhysicsBehaviorPtr;

class PhysicsNotifier {
public:
	virtual void notifyPositionSet(unsigned long id) = 0;
	virtual void notifyColliderUpdate(unsigned long id) = 0;
};

class Manifold {
public:
	Vector2fPtr normal;
	Vector2fPtr delta;
	Vector2fPtr position;
	float time;

	Manifold() {
		normal = std::make_shared<Vector2f>(0.0f, 0.0f);
		delta = std::make_shared<Vector2f>(0.0f, 0.0f);
		position = std::make_shared<Vector2f>(0.0f, 0.0f);
		time = 0.0f;
	}
};

class Sweep {
public:
	ManifoldPtr manifold;
	Vector2fPtr position;
	float time;

	Sweep() {
		manifold = std::make_shared<Manifold>();
		position = std::make_shared<Vector2f>(0.0f, 0.0f);
	}
};

class ColliderShape {
public:
	Vector2f* position;

	ColliderShape(Vector2f* position) {
		this->position = position;
	}

	ColliderShape(const ColliderShape& colliderShape) {
		this->position = GCC_NEW Vector2f(*colliderShape.position);
	}

	ColliderShape(const rapidjson::Value& root) {
		this->position = GCC_NEW Vector2f(root["position"]);
	}

	virtual ~ColliderShape() {
		delete this->position;
	}

	void serialize(Serializer& serializer) const {
		serializer.writer.StartObject();

		serializer.writer.String("position");
		this->position->serialize(serializer);

		this->onSerialize(serializer);

		serializer.writer.EndObject();
	}

	virtual void onSerialize(Serializer& serializer) const = 0;

	virtual bool checkCollision(const ColliderShape& collider) const = 0;

	virtual void intersect(const ColliderShape& collider, Manifold* manifold) const = 0;

	virtual void sweep(const ColliderShape& collider, const Vector2f& delta, Sweep* sweep) const = 0;

	virtual ColliderType colliderType() const = 0;

	virtual void setPosition(const Vector2f& position);
};

class AABBColliderShape : public ColliderShape {
public:
	int width;
	int height;

	AABBColliderShape(Vector2f* position, int width, int height) : ColliderShape(position) {
		this->width = width;
		this->height = height;
	}

	AABBColliderShape(const ColliderShape& colliderShape) : ColliderShape(colliderShape) {
		const AABBColliderShape& aabb = static_cast<const AABBColliderShape&>(colliderShape);
		this->width = aabb.width;
		this->height = aabb.height;
	}

	AABBColliderShape(const rapidjson::Value& root) : ColliderShape(root) {
		this->width = root["width"].GetInt();
		this->height = root["height"].GetInt();
	}

	void onSerialize(Serializer& serializer) const override {
		serializer.writer.String("width");
		serializer.writer.Int(this->width);
		serializer.writer.String("height");
		serializer.writer.Int(this->height);
	}

	bool checkCollision(const ColliderShape& collider) const override;
	void intersect(const ColliderShape& collider, Manifold* manifold) const override;
	void sweep(const ColliderShape& collider, const Vector2f& delta, Sweep* sweep) const override;

	ColliderType colliderType() const override;

	ExtentPtr getExtent() const;
};

class CircleColliderShape : public ColliderShape {
public:
	int radius;

	CircleColliderShape(Vector2f* position, int radius) : ColliderShape(position) {
		this->radius = radius;
	}

	CircleColliderShape(const ColliderShape& colliderShape) : ColliderShape(colliderShape) {
		const CircleColliderShape& circle = static_cast<const CircleColliderShape&>(colliderShape);
		this->radius = circle.radius;
	}

	CircleColliderShape(const rapidjson::Value& root) : ColliderShape(root) {
		this->radius = root["radius"].GetInt();
	}

	void onSerialize(Serializer& serializer) const override {
		serializer.writer.String("radius");
		serializer.writer.Int(this->radius);
	}

	bool checkCollision(const ColliderShape& collider) const override;
	void intersect(const ColliderShape& collider, Manifold* manifold) const override;
	void sweep(const ColliderShape& collider, const Vector2f& delta, Sweep* sweep) const override;

	ColliderType colliderType() const override;
};

class OBBColliderShape : public ColliderShape {
public:
	int width;
	int height;
	int angle;
	Vector2f* corners[4];
	Vector2f* axis[2];
	float origin[2];

	OBBColliderShape(Vector2f* position, int width, int height, int angle) : ColliderShape(position), 
		corners{
			GCC_NEW Vector2f(0, 0),
			GCC_NEW Vector2f(0, 0),
			GCC_NEW Vector2f(0, 0),
			GCC_NEW Vector2f(0, 0) },
		axis{
			GCC_NEW Vector2f(0, 0),
			GCC_NEW Vector2f(0, 0) },
		origin{
			0.0f,
			0.0f }
	{
		this->width = width;
		this->height = height;
		this->angle = angle;
		
		this->setup();
	}

	OBBColliderShape(const ColliderShape& colliderShape) : ColliderShape(colliderShape),
		corners{
		GCC_NEW Vector2f(0, 0),
		GCC_NEW Vector2f(0, 0),
		GCC_NEW Vector2f(0, 0),
		GCC_NEW Vector2f(0, 0) },
		axis{
		GCC_NEW Vector2f(0, 0),
		GCC_NEW Vector2f(0, 0) },
		origin{
		0.0f,
		0.0f } 
	{
		const OBBColliderShape& obb = static_cast<const OBBColliderShape&>(colliderShape);
		this->width = obb.width;
		this->height = obb.height;
		this->angle = obb.angle;

		this->setup();
	}

	OBBColliderShape(const rapidjson::Value& root) : ColliderShape(root),
		corners{
		GCC_NEW Vector2f(0, 0),
		GCC_NEW Vector2f(0, 0),
		GCC_NEW Vector2f(0, 0),
		GCC_NEW Vector2f(0, 0) },
		axis{
		GCC_NEW Vector2f(0, 0),
		GCC_NEW Vector2f(0, 0) },
		origin{
		0.0f,
		0.0f } {
		this->width = root["width"].GetInt();
		this->height = root["height"].GetInt();
		this->angle = root["angle"].GetInt();
	}

	~OBBColliderShape() override {
		for (Vector2f* ptr : this->corners) {
			delete ptr;
		}
		delete[] this->corners;

		for (Vector2f* ptr : this->axis) {
			delete ptr;
		}
		delete[] this->axis;
	}

	void onSerialize(Serializer& serializer) const override {
		serializer.writer.String("width");
		serializer.writer.Int(this->width);
		serializer.writer.String("height");
		serializer.writer.Int(this->height);
		serializer.writer.String("angle");
		serializer.writer.Int(this->angle);
	}

	bool checkCollision(const ColliderShape& collider) const override;
	void intersect(const ColliderShape& collider, Manifold* manifold) const override;

	ColliderType colliderType() const override;

	void setPosition(const Vector2f& position) override;
	void sweep(const ColliderShape& collider, const Vector2f& delta, Sweep* sweep) const override;

	void setAngle(int angle);

private:
	void setup();
	bool overlapOneWay(const OBBColliderShape& obb) const;
};

class RayColliderShape : public ColliderShape {
public:
	Vector2fPtr end;

	RayColliderShape(Vector2f* begin, Vector2f* end) : ColliderShape(begin) {
		this->end = Vector2fPtr(end);
	}

	RayColliderShape(const ColliderShape& colliderShape) : ColliderShape(colliderShape) {
		const RayColliderShape& ray = static_cast<const RayColliderShape&>(colliderShape);
		this->end = ray.end;
	}

	RayColliderShape(const rapidjson::Value& root) : ColliderShape(root) {
		// TODO
	}

	void onSerialize(Serializer& serializer) const override {
		// TODO
	}

	bool checkCollision(const ColliderShape& collider) const override;
	void intersect(const ColliderShape& collider, Manifold* manifold) const override;
	void sweep(const ColliderShape& collider, const Vector2f& delta, Sweep* sweep) const override;

	ColliderType colliderType() const override {
		return ColliderType::RAY;
	}
};

class Collider {
public:
	ColliderShape* colliderShape;

	Collider(ColliderShape* colliderShape);

	Collider(const Collider& copy);

	Collider(const rapidjson::Value& root);

	~Collider();

	void setOnCollisionCallback(std::function<void(const Collider&)>& callback);

	bool checkCollision(const Collider& collider) const;

	void onCollision(const Collider& collider) const;

	void serialize(Serializer& serializer) const {
		serializer.writer.StartObject();

		serializer.writer.String("colliderShape");
		this->colliderShape->serialize(serializer);

		serializer.writer.EndObject();
	}

private:
	std::function<void(const Collider&)> onCollisionCallback;
};

class Body {
public:

	int id;
	float mass;
	float speed;
	Vector2f* position;
	Vector2f* velocity;
	float width;
	float height;
	Collider* collider;
	Target* target;
	string tag;

	Body(int id, float x, float y, float width, float height);

	Body(const rapidjson::Value& root);

	Body(const Body& copy);

	~Body();

	bool checkPoint(const Vector2f& point);

	void setSpeed(float speed);
	float getSpeed() const;

	void setMass(float mass);
	float getMass();

	void setPosition(const Vector2f& position);
	const Vector2f& getPosition();

	void setVelocity(const Vector2f& vector);
	const Vector2f& getVelocity();

	void setCollider(Collider* collider);
	const Collider& getCollider();
	bool isCollidable();

	void setTarget(Target* target);
	const Target& getTarget() const;
	bool hasTarget() const;

	float getWidth();
	float getHeight();
	void setWidth(float width);
	void setHeight(float height);

	const string& getTag();
	void setTag(const string& tag);

	void serialize(Serializer& serializer) const {
		serializer.writer.StartObject();

		serializer.writer.String("speed");
		serializer.writer.Double(speed);

		serializer.writer.String("mass");
		serializer.writer.Double(mass);

		serializer.writer.String("position");
		position->serialize(serializer);

		serializer.writer.String("velocity");
		velocity->serialize(serializer);

		serializer.writer.String("width");
		serializer.writer.Double(width);

		serializer.writer.String("height");
		serializer.writer.Double(height);

		serializer.writer.String("tag");
		serializer.writer.String(tag.c_str());

		if (this->collider != nullptr) {
			serializer.writer.String("collider");
			this->collider->serialize(serializer);
		}

		serializer.writer.EndObject();
	}
};

class QuadtreeNode {
public:
	QuadtreeNode(float x, float y, float width, float height);
	~QuadtreeNode() {
		if (this->mBody != nullptr) {
			delete mBody;
		}
	}
	std::vector<QuadtreeNode*> children;
	std::unordered_set<Body*> bodiesContainer;

	void pushChild(QuadtreeNode* node) {
		children.push_back(node);
	}

	bool leaf{ false };

	bool check(const Body& body);
	void add(Body* body);
	void remove(const Body& body);
	bool contains(const Body& body);

	void clear();

	const ExtentPtr getNodeExtent();

private:
	Body* mBody;
};

class Quadtree {
public:
	const static int MIN_SIZE{ 100 };

	Quadtree(float x, float y, float width, float height);
	~Quadtree();
	void addBody(Body* body);
	void removeBody(Body& body);
	void getCollidingBodies(const Body& body, BodyList& bodies);
	void getNeigboringBodies(const Body& body, BodyList& bodies);
	void clear();

private:
	void addHelper(Body& body, QuadtreeNode& node);
	void removeHelper(Body& body, QuadtreeNode& node);
	void createTree(QuadtreeNode* parent);
	QuadtreeNode* mRootNode{ nullptr };
	vector<QuadtreeNode*> mLeaves;
};

class Target {
public:
	virtual const Vector2f& getTargetPosition() const = 0;

	void setThreshold(float threshold) {
		this->mThreshold = threshold;
	}

	float getThreshold() const {
		return this->mThreshold;
	}

private:
	float mThreshold = 2;
};

class BodyTarget : public Target {
public:
	BodyTarget(BodyPtr body);

	const Vector2f& getTargetPosition() const override;

private:
	BodyPtr mBody;
};

class PositionTarget : public Target {
public:
	PositionTarget(Vector2fPtr position);

	const Vector2f& getTargetPosition() const override;

private:
	Vector2fPtr mPosition;
};

class PhysicsComponent : public Component {
public:
	PhysicsComponent(unsigned long entityId, BodyPtr body) : Component(entityId, ComponentType::PHYSICS_COMPONENT) {
		mBody = body;
	}

	PhysicsComponent(unsigned long entityId, const rapidjson::Value& root) : Component(entityId, ComponentType::PHYSICS_COMPONENT) {
		const rapidjson::Value& body = root["mBody"];
		mBody = BodyPtr(GCC_NEW Body(body));
		mBody->id = entityId;
	}

	void setCollider(Collider* collider);
	bool isCollidable();
	void setPosition(const Vector2f& position);
	const Vector2f& getPosition();
	void setVelocity(const Vector2f& velocity);
	const Vector2f& getVelocity();

	void setSpeed(float speed);
	float getSpeed();

	void setSize(float width, float height);
	float getWidth();
	float getHeight();

	const string& getTag();
	void setTag(const string& tag);

	BodyPtr getBody() {
		return mBody;
	}

	void setTarget(Target* target) {
		mBody->setTarget(target);
	}

	const Target& getTarget() {
		return mBody->getTarget();
	}

	bool hasTarget() {
		return mBody->hasTarget();
	}

	void serialize(Serializer& serializer) const {
		serializer.writer.StartObject();

		serializer.writer.String("componentId");
		serializer.writer.Uint((Uint8)componentId);

		serializer.writer.String("mBody");
		mBody->serialize(serializer);

		serializer.writer.EndObject();
	}

private:
	BodyPtr mBody;
};

class PhysicsBehavior {
public:
	/* return true if the body was updated. */
	virtual bool updateBehavior(float step, Body& body, QuadtreePtr quadtree) = 0;
};

class BasicBehavior : public PhysicsBehavior {
public:
	bool updateBehavior(float step, Body& body, QuadtreePtr quadtree) override;
private:
	void handleCollision(const Vector2f& delta, Vector2f& newPosition, Body& body, QuadtreePtr quadtree);
};

class SteeringBehavior : public PhysicsBehavior {
public:
	bool updateBehavior(float step, Body& body, QuadtreePtr quadtree) override;
private:
	Vector2f applySteeringToVelocity(Body& body, float step);
};

#endif // !__PHYSICS_H__
