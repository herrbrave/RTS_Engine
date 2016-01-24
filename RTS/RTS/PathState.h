#ifndef __PATH_STATE_H__
#define __PATH_STATE_H__

#include"Flowfield.h"
#include"State.h"
#include"Tower.h"
#include"Unit.h"

class PathState : public State {
public:
	PathState(std::shared_ptr<Unit> unit, std::shared_ptr<Flowfield> flowField, std::vector<std::shared_ptr<Tower>> * towers);
	virtual void begin();
	virtual void update();
	virtual void end();

	virtual bool hasBegun();
	virtual bool isEnded();

private:
	static int const MAX_SEE_AHEAD = 10;

	std::shared_ptr<Flowfield> mFlowfield;
	std::shared_ptr<Unit> mUnit;
	std::vector<std::shared_ptr<Tower>>* mTowers;
	bool mStarted{ false };
	bool mEnded{ false };
};

#endif // !__PATH_STATE_H__
