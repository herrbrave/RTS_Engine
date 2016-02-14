#ifndef __STATE_H__
#define __STATE_H__

#include<functional>

class State {
public:
	virtual void begin() = 0;
	virtual void update() = 0;
	virtual void end() = 0;

	virtual bool hasBegun() = 0;
	virtual bool isEnded() = 0;
};

#endif // !__STATE_H__
