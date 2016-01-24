#include"ShootState.h"

void ShootState::end() {
	std::static_pointer_cast<Unit>(mTarget)->damage(5);
	mDone = true;
}