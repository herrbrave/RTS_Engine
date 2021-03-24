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


struct Extent {
	float x0, y0, x1, y1;
};
typedef shared_ptr<Extent> ExtentPtr;
typedef weak_ptr<Extent> WeakExtentPtr;

class Collider;
typedef shared_ptr<Collider> ColliderPtr;
typedef weak_ptr<Collider> WeakColliderPtr;

class Body;
typedef shared_ptr<Body> BodyPtr;
typedef weak_ptr<Body> WeakBodyPtr;

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

class Collider {
public:
	float width;
	float height;
	Vector2fPtr position;

	Collider(float x, float y, float width, float height);

	Collider(const Collider& copy);

	void setOnCollisionCallback(std::function<void(const Collider&)>& callback);

	bool checkCollision(const Collider& collider) const;

	void onCollision(const Collider& collider) const;

	ExtentPtr getExtent() const;

private:
	std::function<void(const Collider&)> onCollisionCallback;
};

class Body {
public:

	int id;
	float mass;
	float speed;
	Vector2fPtr position{ nullptr };
	Vector2fPtr velocity{ nullptr };
	float width;
	float height;
	ColliderPtr collider;
	TargetPtr target;
	string tag;

	Body(int id, float x, float y, float width, float height);

	Body(const rapidjson::Value& root);

	Body(const Body& copy);

	bool checkPoint(const Vector2f& point);

	void setSpeed(float speed);
	float getSpeed();

	void setMass(float mass);
	float getMass();

	void setPosition(const Vector2f& position);
	const Vector2f& getPosition();

	void setVelocity(const Vector2f& vector);
	const Vector2f& getVelocity();

	void setCollider(Collider* collider);
	WeakColliderPtr getCollider();
	bool isCollidable();

	void setTarget(TargetPtr target);
	WeakTargetPtr getTarget();
	bool hasTarget();

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

		serializer.writer.EndObject();
	}
};

class QuadtreeNode {
public:
	QuadtreeNode(float x, float y, float width, float height);
	std::vector<QuadtreeNodePtr> children;
	std::unordered_set<BodyPtr> bodiesContainer;

	void pushChild(QuadtreeNodePtr node) {
		children.push_back(node);
	}

	bool leaf{ false };

	bool check(BodyPtr body);
	void add(BodyPtr body);
	void remove(BodyPtr body);
	bool contains(BodyPtr body);

	void clear();

	ExtentPtr getNodeExtent();

private:
	BodyPtr mBody{ nullptr };
};

class Quadtree {
public:
	const static int MIN_SIZE{ 100 };

	Quadtree(float x, float y, float width, float height);
	void addBody(BodyPtr body);
	void removeBody(BodyPtr body);
	void getCollidingBodies(BodyPtr body, vector<WeakBodyPtr>& bodies);
	void getNeigboringBodies(BodyPtr body, vector<BodyPtr>& bodies);
	void clear();

private:
	void addHelper(BodyPtr body, QuadtreeNodePtr node);
	void removeHelper(BodyPtr body, QuadtreeNodePtr node);
	void createTree(QuadtreeNodePtr parent);
	QuadtreeNodePtr mRootNode{ nullptr };
	vector<QuadtreeNodePtr> mLeaves;
};

class Target {
public:
	virtual const Vector2f& getTargetPosition() = 0;

	void setThreshold(float threshold) {
		this->mThreshold = threshold;
	}

	float getThreshold() {
		return this->mThreshold;
	}

private:
	float mThreshold = 2;
};

class BodyTarget : public Target {
public:
	BodyTarget(BodyPtr body);

	const Vector2f& getTargetPosition() override;

private:
	BodyPtr mBody;
};

class PositionTarget : public Target {
public:
	PositionTarget(Vector2fPtr position);

	const Vector2f& getTargetPosition() override;

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

	WeakBodyPtr getBody() {
		return WeakBodyPtr(mBody);
	}

	void setTarget(TargetPtr target) {
		mBody->setTarget(target);
	}

	WeakTargetPtr getTarget() {
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
	virtual bool updateBehavior(float step, BodyPtr& body, QuadtreePtr quadtree) = 0;
};

class BasicBehavior : public PhysicsBehavior {
public:
	bool updateBehavior(float step, BodyPtr& body, QuadtreePtr quadtree) override;
};

class SteeringBehavior : public PhysicsBehavior {
public:
	bool updateBehavior(float step, BodyPtr& body, QuadtreePtr quadtree) override;
private:
	Vector2f applySteeringToVelocity(BodyPtr& body, float step);
};

#endif // !__PHYSICS_H__
