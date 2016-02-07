#ifndef __ENTITY_H__
#define __ENTITY_H__

#include<atomic>
#include<vector>

#include"Graphics.h"
#include "Physics.h"
#include"State.h"
#include"vector2f.h"

static std::atomic_ulong sEntityId{ 1 };

class Entity {
public:
	Entity(Body* body, Drawable* drawable);

	virtual void update();
	virtual void onUpdate() = 0;
	void pushState(std::shared_ptr<State> state);

	Body* getBody();
	Drawable* getDrawable();

	inline const unsigned long getId() {
		return mId;
	}
protected:
	std::vector<std::shared_ptr<State>> mStateQueue;
	std::shared_ptr<Body> mBody;
	std::shared_ptr<Drawable> mDrawable;
	const unsigned long mId = sEntityId.fetch_add(1);
};

class EntityFactory {
public:
	EntityFactory(GraphicsSystem* graphics, PhysicsSystem* physics);

protected:
	std::shared_ptr<GraphicsSystem> mGraphicsSystem{ nullptr };
	std::shared_ptr<PhysicsSystem> mPhysicsSystem{ nullptr };
};

#endif // !__ENTITY_H__
