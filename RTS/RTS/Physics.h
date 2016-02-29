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
	Body(float x, float y, float width, float height) {
		mSpeed = 0;
		mPosition.reset(new vector2f(x, y));
		mVelocity.reset(new vector2f(0, 0));
		mWidth = width;
		mHeight = height;
	}

	Body(const rapidjson::Value& root) {
		mSpeed = root["mSpeed"].GetDouble();
		mPosition.reset(new vector2f(root["mPosition"]));
		mVelocity.reset(new vector2f(root["mVelocity"]));
		mWidth = root["mWidth"].GetDouble();
		mHeight = root["mHeight"].GetDouble();
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

	virtual void serialize(Serializer& serializer) const {
		serializer.writer.StartObject();

		onSerialization(serializer);

		serializer.writer.String("mSpeed");
		serializer.writer.Double(mSpeed);

		serializer.writer.String("mPosition");
		mPosition->serialize(serializer);

		serializer.writer.String("mVelocity");
		mVelocity->serialize(serializer);

		serializer.writer.String("mWidth");
		serializer.writer.Double(mWidth);

		serializer.writer.String("mHeight");
		serializer.writer.Double(mHeight);

		serializer.writer.EndObject();
	}

protected:
	virtual void onSerialization(Serializer& serializer) const = 0;

	float mSpeed;
	p_vector2f mPosition{ nullptr };
	p_vector2f mVelocity{ nullptr };
	float mWidth;
	float mHeight;
};

class BlockBody : public Body {
public:
	BlockBody(float x, float y, float width, float height);

	BlockBody(const rapidjson::Value& root) : Body(root) {}

	bool checkPoint(vector2f& point) override;
	bool checkCollision(Body& body) override;
	Extent getExtent() override;

protected:
	void onSerialization(Serializer& serializer) const override;
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

class PhysicsComponent : public Component {
public:
	PhysicsComponent(unsigned long entityId, Body* body) : Component(entityId, ComponentType::PHYSICS_COMPONENT) {
		mBody = body;
	}

	PhysicsComponent(unsigned long entityId, const rapidjson::Value& root) : Component(entityId, ComponentType::PHYSICS_COMPONENT) {
		const rapidjson::Value& body = root["mBody"];
		std::string bodyType = body["BodyType"].GetString();
		if (bodyType == "BlockBody") {
			mBody = new BlockBody(body);
		}
		else {
			assert(false);
		}
	}

	void setPosition(vector2f* position);
	const vector2f* getPosition();
	void setVelocity(vector2f* velocity);
	const vector2f* getVelocity();

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
};

#endif // !__PHYSICS_H__
