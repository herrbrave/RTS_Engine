#ifndef __PHYSICS_H__
#define __PHYSICS_H__

#include"vector2f.h"

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
protected:
	float mSpeed;
	p_vector2f mPosition{ nullptr };
	p_vector2f mVelocity{ nullptr };
};

class BlockBody : public Body {
public:
	BlockBody(float x, float y, float width, float height);

	bool checkPoint(vector2f& point) override;
	bool checkCollision(Body& body) override;
	Extent getExtent() override;

private:
	float mWidth;
	float mHeight;
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

	Body* getBody(const unsigned long id);

	void registerBody(const unsigned long id, Body* body);
	void deregisterBody(const unsigned long id);

	void update(Uint32 delta);

private:
	std::unordered_map<unsigned long, std::shared_ptr<Body>> mBodies;
};

#endif // !__PHYSICS_H__
