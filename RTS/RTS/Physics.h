#ifndef __PHYSICS_H__
#define __PHYSICS_H__

#include"Component.h"
#include"vector2f.h"

#include<functional>
#include<map>
#include<SDL.h>
#include<stack>
#include<unordered_map>
#include<unordered_set>

struct Extent {
	float x0, y0, x1, y1;
};

class PhysicsNotifier {
public:
	virtual void notifyPositionSet(unsigned long id) = 0;
	virtual void notifyColliderUpdate(unsigned long id) = 0;
};

class Collider {
public:
	float width;
	float height;
	p_vector2f position{ nullptr };

	Collider(float x, float y, float width, float height);

	Collider(const Collider& copy);

	void setOnCollisionCallback(std::function<void(const Collider&)>& callback);

	bool checkCollision(const Collider& collider) const;

	void onCollision(const Collider& collider) const;

	void getExtent(Extent& extent) const;

private:
	std::function<void(const Collider&)> onCollisionCallback;
};

class Body {
public:

	float speed;
	p_vector2f position{ nullptr };
	p_vector2f velocity{ nullptr };
	float width;
	float height;
	std::unique_ptr<Collider> collider;

	Body(float x, float y, float width, float height);

	Body(const rapidjson::Value& root);

	Body(const Body& copy);

	bool checkPoint(const vector2f& point);

	void setSpeed(float speed);

	float getSpeed();

	void setPosition(vector2f* position);
	const vector2f* getPosition();

	void setVelocity(vector2f* vector);
	const vector2f* getVelocity();

	void setCollider(Collider* collider);
	const Collider& getCollider();
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
	std::vector<QuadtreeNode*> children;
	std::unordered_set<Body*> bodiesContainer;

	void pushChild(QuadtreeNode* node) {
		children.push_back(node);
	}

	bool leaf{ false };

	bool check(Body* body);
	void add(Body* body);
	void remove(Body* body);
	bool contains(Body* body);

	void clear();

	void getNodeExtent(Extent& extent);

private:
	std::unique_ptr<Body> mBody{ nullptr };
};

class Quadtree {
public:
	const static int MIN_SIZE{ 100 };

	Quadtree(float x, float y, float width, float height);
	void addBody(Body* body);
	void removeBody(Body* body);
	void getCollidingBodies(Body* body, std::vector<Body*>& bodies);
	void getNeigboringBodies(Body* body, std::vector<Body*>& bodies);
	void clear();

private:
	void addHelper(Body* body, QuadtreeNode* node);
	void removeHelper(Body* body, QuadtreeNode* node);
	void createTree(QuadtreeNode* parent);
	std::unique_ptr<QuadtreeNode> mRootNode{ nullptr };
	std::vector<QuadtreeNode*> mLeaves;
};

class PhysicsComponent : public Component {
public:
	PhysicsComponent(unsigned long entityId, Body* body, PhysicsNotifier* physicsNotifier) : Component(entityId, ComponentType::PHYSICS_COMPONENT) {
		mBody = body;
		mPhysicsNotifier = physicsNotifier;
	}

	PhysicsComponent(unsigned long entityId, const rapidjson::Value& root, PhysicsNotifier* physicsNotifier) : Component(entityId, ComponentType::PHYSICS_COMPONENT) {
		const rapidjson::Value& body = root["mBody"];
		mBody = new Body(body);
		mPhysicsNotifier = physicsNotifier;
	}

	void setCollider(Collider* collider);
	bool isCollidable();
	void setPosition(vector2f* position);
	const vector2f* getPosition();
	void setVelocity(vector2f* velocity);
	const vector2f* getVelocity();

	void setSpeed(float speed);
	float getSpeed();

	void setSize(float width, float height);
	float getWidth();
	float getHeight();
	Body* getBody() {
		return mBody;
	}

	void serialize(Serializer& serializer) const {
		serializer.writer.StartObject();

		serializer.writer.String("componentId");
		serializer.writer.Uint(componentId);

		serializer.writer.String("mBody");
		mBody->serialize(serializer);

		serializer.writer.EndObject();
	}

private:
	Body* mBody;
	PhysicsNotifier* mPhysicsNotifier;
};

#endif // !__PHYSICS_H__
