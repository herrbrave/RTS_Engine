#ifndef __STATE_H__
#define __STATE_H__

#include"Constants.h"
#include<functional>

class State;
typedef shared_ptr<State> StatePtr;
typedef weak_ptr<State> WeakStatePtr;

class State {
public:
	virtual void begin() = 0;
	virtual void update() = 0;
	virtual void end() = 0;

	virtual bool hasBegun() = 0;
	virtual bool isEnded() = 0;
};

#endif // !__STATE_H__
