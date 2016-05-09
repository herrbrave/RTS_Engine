#ifndef __PHYSICS_H__
#define __PHYSICS_H__

#include"Component.h"
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

class PhysicsNotifier;
typedef shared_ptr<PhysicsNotifier> PhysicsNotifierPtr;
typedef weak_ptr<PhysicsNotifier> WeakPhysicsNotifierPtr;

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

class PhysicsComponent;
typedef shared_ptr<PhysicsComponent> PhysicsComponentPtr;
typedef weak_ptr<PhysicsComponent> WeakPhysicsComponentPtr;

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

	void setOnCollisionCallback(std::function<void(ColliderPtr)>& callback);

	bool checkCollision(ColliderPtr collider) const;

	void onCollision(ColliderPtr collider) const;

	ExtentPtr getExtent() const;

private:
	std::function<void(ColliderPtr)> onCollisionCallback;
};

class Body {
public:

	float speed;
	Vector2fPtr position{ nullptr };
	Vector2fPtr velocity{ nullptr };
	float width;
	float height;
	ColliderPtr collider;

	Body(float x, float y, float width, float height);

	Body(const rapidjson::Value& root);

	Body(const Body& copy);

	bool checkPoint(const Vector2f& point);

	void setSpeed(float speed);

	float getSpeed();

	void setPosition(Vector2fPtr position);
	WeakVector2fPtr getPosition();

	void setVelocity(Vector2fPtr vector);
	WeakVector2fPtr getVelocity();

	void setCollider(ColliderPtr collider);
	WeakColliderPtr getCollider();
	bool isCollidable();

	float getWidth();
	float getHeight();
	void setWidth(float width);
	void setHeight(float height);

	void serialize(Serializer& serializer) const {
		serializer.writer.StartObject();

		serializer.writer.String("speed");
		serializer.writer.Double(speed);

		serializer.writer.String("position");
		position->serialize(serializer);

		serializer.writer.String("velocity");
		velocity->serialize(serializer);

		serializer.writer.String("width");
		serializer.writer.Double(width);

		serializer.writer.String("height");
		serializer.writer.Double(height);

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

class PhysicsComponent : public Component {
public:
	PhysicsComponent(unsigned long entityId, BodyPtr body, PhysicsNotifierPtr physicsNotifier) : Component(entityId, ComponentType::PHYSICS_COMPONENT) {
		mBody = body;
		mPhysicsNotifier = physicsNotifier;
	}

	PhysicsComponent(unsigned long entityId, const rapidjson::Value& root, PhysicsNotifierPtr physicsNotifier) : Component(entityId, ComponentType::PHYSICS_COMPONENT) {
		const rapidjson::Value& body = root["mBody"];
		mBody = BodyPtr(GCC_NEW Body(body));
		mPhysicsNotifier = physicsNotifier;
	}

	void setCollider(ColliderPtr collider);
	bool isCollidable();
	void setPosition(Vector2fPtr position);
	WeakVector2fPtr getPosition();
	void setVelocity(Vector2fPtr velocity);
	WeakVector2fPtr getVelocity();

	void setSpeed(float speed);
	float getSpeed();

	void setSize(float width, float height);
	float getWidth();
	float getHeight();
	WeakBodyPtr getBody() {
		return WeakBodyPtr(mBody);
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
	PhysicsNotifierPtr mPhysicsNotifier;
};

#endif // !__PHYSICS_H__
