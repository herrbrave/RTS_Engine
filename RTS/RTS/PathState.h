#ifndef __PATH_STATE_H__
#define __PATH_STATE_H__

#include"Flowfield.h"
#include"State.h"
#include"Entity.h"

class PathState : public State {
public:
	PathState(EntityPtr unit, FlowfieldPtr flowField);
	virtual void begin();
	virtual void update();
	virtual void end();

	virtual bool hasBegun();
	virtual bool isEnded();

private:
	static int const MAX_SEE_AHEAD = 10;

	FlowfieldPtr mFlowfield;
	EntityPtr mUnit;
	bool mStarted{ false };
	bool mEnded{ false };
};

#endif // !__PATH_STATE_H__
