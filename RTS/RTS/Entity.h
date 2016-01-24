#ifndef __ENTITY_H__
#define __ENTITY_H__

#include<vector>

#include"Block.h"
#include"Camera.h"
#include "Physics.h"
#include"State.h"
#include"vector2f.h"

class Entity : public Body {
public:
	Entity(int x, int y, int width, int height, Uint8 r, Uint8 g, Uint8 b, std::shared_ptr<Camera> camera);

	virtual void update();
	virtual void draw(std::shared_ptr<SDL_Renderer> renderer);
	const vector2f& getLocation();
	void setLocation(vector2f& location);
	void pushState(std::shared_ptr<State> state);

	std::unique_ptr<Block> mBlock{ nullptr };

	inline int getNumStates() {
		return mStateQueue.size();
	}

	/* Physics implementation */
	virtual bool checkPoint(vector2f& point);
	virtual bool checkCollision(Body& body);
	virtual Extent getExtent();

protected:
	std::shared_ptr<Camera> mCamera{ nullptr };
	std::vector<std::shared_ptr<State>> mStateQueue{};
};

#endif // !__ENTITY_H__
