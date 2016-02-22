#ifndef __EVENT_H__
#define __EVENT_H__

#include"vector2f.h"

enum MouseButton {
	LEFT = 0,
	RIGHT = 1,
	MIDDLE = 2
};

enum MouseAction {
	MOVE = 0,
	CLICK_DOWN = 1,
	CLICK_UP = 2
};

struct MouseEvent {
	MouseButton button;
	MouseAction action;
	p_vector2f position{ new vector2f(0, 0) };
};

#endif // !__EVENT_H__
