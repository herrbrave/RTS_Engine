#ifndef __SHOOT_STATE__
#define __SHOOT_STATE__

#include"MoveState.h"

class ShootState : public MoveState {
public:
	ShootState(std::shared_ptr<Entity> entity, std::shared_ptr<Entity> target) : MoveState(entity, target) {
		mSpeed = 25;
	}

protected:
	virtual void end();
};

#endif // !__SHOOT_STATE__
