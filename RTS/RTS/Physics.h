#ifndef __PHYSICS_H__
#define __PHYSICS_H__

#include"Component.h"
#include"vector2f.h"

#include<map>
#include<SDL.h>
#include<unordered_map>
#include<unordered_set>

struct Extent {
	float x0, y0, x1, y1;
};

class Body {
public:
	Body() {
		mSpeed = 0;
		mPosition.reset(new vector2f(0, 0));
		mVelocity.reset(new vector2f(0, 0));
	}

	virtual bool checkPoint(vector2f& point) = 0;
	virtual bool checkCollision(Body& body) = 0;
	virtual Extent getExtent() = 0;

	void setSpeed(float speed) {
		mSpeed = speed;
	}

	float getSpeed() {
		return mSpeed;
	}

	void setPosition(vector2f* position) {
		mPosition->set(position);
	}
	const vector2f* getPosition() {
		return mPosition.get();
	}

	void setVelocity(vector2f* vector) {
		mVelocity->set(vector);
	}
	const vector2f* getVelocity() {
		return mVelocity.get();
	}

	float getWidth();
	float getHeight();
	void setWidth(float width);
	void setHeight(float height);

protected:
	float mSpeed;
	p_vector2f mPosition{ nullptr };
	p_vector2f mVelocity{ nullptr };
	float mWidth;
	float mHeight;
};

class BlockBody : public Body {
public:
	BlockBody(float x, float y, float width, float height);

	bool checkPoint(vector2f& point) override;
	bool checkCollision(Body& body) override;
	Extent getExtent() override;
};

class QuadtreeNode {
public:
	QuadtreeNode(float x, float y, float width, float height);
	std::vector<std::shared_ptr<QuadtreeNode>> children;
	std::unordered_set<Body*> bodiesContainer;

	void pushChild(QuadtreeNode* node) {
		children.push_back(std::shared_ptr<QuadtreeNode>(node));
	}

	bool leaf{ false };

	bool check(Body* body);
	void add(Body* body);
	void remove(Body* body);
	bool contains(Body* body);

	Extent getNodeExtent();

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

private:
	void addHelper(Body* body, QuadtreeNode* node);
	void removeHelper(Body* body, QuadtreeNode* node);
	void createTree(QuadtreeNode* parent);
	std::unique_ptr<QuadtreeNode> mRootNode{ nullptr };
	std::vector<QuadtreeNode*> mLeaves;
};

class PhysicsSystem {
public:

	PhysicsSystem(){}
	~PhysicsSystem() = default;

	Body* getBody(const unsigned long id);

	void registerBody(const unsigned long id, Body* body);
	void deregisterBody(const unsigned long id);

	void update(Uint32 delta);

private:
	std::map<unsigned long, std::shared_ptr<Body>> mBodies;
};

static const unsigned long PHYSICS_COMPONENT_TYPE = sComponentId.fetch_add(1);

class PhysicsComponent : public Component {
public:
	PhysicsComponent(unsigned long entityId, Body* body) : Component(entityId, PHYSICS_COMPONENT_TYPE) {
		mBody = body;
	}

	void setPosition(vector2f* position);
	const vector2f* getPosition();
	void setVelocity(vector2f* velocity);
	const vector2f* getVelocity();

	void setSize(float width, float height);
	float getWidth();
	float getHeight();

private:
	Body* mBody;
};

#endif // !__PHYSICS_H__
