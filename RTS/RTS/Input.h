#ifndef __EVENT_H__
#define __EVENT_H__

#include<functional>
#include<unordered_map>
#include<SDL.h>

#include"Component.h"
#include"vector2f.h"

enum class MouseButton {
	LEFT = 0,
	RIGHT = 1,
	MIDDLE = 2,
	NONE = 3
};

enum class MouseAction {
	MOVE = 0,
	CLICK_DOWN = 1,
	CLICK_UP = 2
};

struct MouseEvent {
	MouseButton button; 
	MouseAction action;
	p_vector2f position{ new vector2f(0, 0) };
};

enum class MouseState {
	UP = 0,
	OVER = 1,
	DOWN = 2,
};

struct KeyboardEvent {
	Sint32 key;
	bool ctrlDown;
	bool shiftDown;
};

struct Event {
	MouseEvent* mouseEvent;
	KeyboardEvent* keyEvent;
};

enum class EventType {
	MOUSE_MOVE = 0,
	MOUSE_BUTTON_DOWN = 1,
	MOUSE_BUTTON_UP = 2,
	KEY_DOWN = 3,
	KEY_UP = 4,

	NONE = -1,
	/* Add joystick stuff here later. */
};

enum class InputEvent {
	ON_MOUSE_MOVE = 0,
	ON_MOUSE_ENTER = 1,
	ON_MOUSE_EXIT = 2,
	ON_CLICK = 3,
	ON_DRAG = 4,
	ON_KEY_DOWN = 5,
	ON_KEY_UP = 6,
};

class MouseMovementHandler {
public:
	virtual bool checkForMouseOver(unsigned long id, const vector2f& position) = 0;
};

class InputListener {
public:
	unsigned long id;
	MouseState mouseState;
	// TODO: Add Layers.
	std::unordered_map<InputEvent, std::function<bool(Event*)>> eventCallbacks;

	InputListener(unsigned long id) {
		this->id = id;
		mouseState = MouseState::UP;
	}

	bool onEvent(EventType eventType, Event* evt, MouseMovementHandler* mouseMovementHandler);
};

class InputComponent : public Component {
public:
	InputComponent(unsigned long entityId, InputListener* inputListener) : Component(entityId, ComponentType::INPUT_COMPONENT) {
		mInputListener = inputListener;
	}

	void setInputCallback(InputEvent inputEvent, std::function<bool(Event*)>& callback);

	void serialize(Serializer& serializer) const override {

	}

private:
	InputListener* mInputListener;
};

#endif // !__EVENT_H__
