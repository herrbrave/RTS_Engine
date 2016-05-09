#ifndef __MOVE_STATE_H__
#define __MOVE_STATE_H__

#include"State.h"
#include"Entity.h"
#include"Physics.h"

class MoveState : public State {
public:
	virtual void begin();
	virtual void update();
	virtual void end();
	virtual bool hasBegun();
	virtual bool isEnded();
	MoveState(EntityPtr entity, EntityPtr target);

protected:
	EntityPtr mEntity;
	EntityPtr mTarget;
	float mSpeed;
	bool mHasBegun{ false };
	bool mDone{ false };
};


#endif // !__MOVE_STATE_H__
