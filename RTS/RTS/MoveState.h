#ifndef __MOVE_STATE_H__
#define __MOVE_STATE_H__

#include"State.h"
#include"Unit.h"

class MoveState : public State {
public:
	virtual void begin();
	virtual void update();
	virtual void end();
	virtual bool hasBegun();
	virtual bool isEnded();
	MoveState(std::shared_ptr<Entity> entity, std::shared_ptr<Entity> target);

protected:
	std::shared_ptr<Entity> mEntity;
	std::shared_ptr<Entity> mTarget;
	float mSpeed;
	bool mHasBegun{ false };
	bool mDone{ false };
};


#endif // !__MOVE_STATE_H__
