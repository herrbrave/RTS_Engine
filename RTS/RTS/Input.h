#ifndef __INPUT_H__
#define __INPUT_H__

#include<functional>
#include<unordered_map>
#include<SDL.h>

#include"Component.h"
#include"Vector2f.h"

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
	Vector2fPtr position{ GCC_NEW Vector2f(0, 0) };
};
typedef shared_ptr<MouseEvent> MouseEventPtr;
typedef weak_ptr<MouseEvent> WeakMouseEventPtr;

enum class MouseState {
	UP = 0,
	OVER = 1,
	DOWN = 2,
};

enum class KeyAction {
	UP = 0,
	DOWN = 1,
};

struct KeyboardEvent {
	Sint32 key;
	bool ctrlDown;
	bool shiftDown;
};
typedef shared_ptr<KeyboardEvent> KeyboardEventPtr;
typedef weak_ptr<KeyboardEvent> WeakKeyboardEventPtr;

struct InputEvent {
	MouseEventPtr mouseEvent;
	KeyboardEventPtr keyEvent;
};
typedef shared_ptr<InputEvent> EventPtr;
typedef weak_ptr<InputEvent> WeakEventPtr;

enum class InputEventType {
	MOUSE_MOVE = 0,
	MOUSE_BUTTON_DOWN = 1,
	MOUSE_BUTTON_UP = 2,
	KEY_DOWN = 3,
	KEY_UP = 4,

	NONE = -1,
	/* Add joystick stuff here later. */
};

enum class Input {
	ON_MOUSE_ENTER = 1,
	ON_MOUSE_EXIT = 2,
	ON_MOUSE_DOWN = 3,
	ON_CLICK = 4,
	ON_DRAG = 5,
	ON_KEY_DOWN = 6,
	ON_KEY_UP = 7,
};

class MouseMovementHandler;
typedef shared_ptr<MouseMovementHandler> MouseMovementHandlerPtr;
typedef weak_ptr<MouseMovementHandler> WeakMouseMovementHandlerPtr;

class InputListener;
typedef shared_ptr<InputListener>InputListenerPtr;
typedef weak_ptr<InputListener> WeakInputListenerPtr;

class InputComponent;
typedef shared_ptr<InputComponent>InputComponentPtr;
typedef weak_ptr<InputComponent> WeakInputComponentPtr;

class MouseMovementHandler {
public:
	virtual bool checkForMouseOver(unsigned long id, const Vector2f& position) = 0;
};

class InputListener {
public:
	unsigned long id;
	MouseState mouseState;
	// TODO: Add Layers.
	std::unordered_map<Input, std::function<bool(EventPtr)>> eventCallbacks;

	InputListener(unsigned long id) {
		this->id = id;
		mouseState = MouseState::UP;
	}

	bool onEvent(InputEventType eventType, EventPtr evt, MouseMovementHandlerPtr mouseMovementHandler);
};

class InputComponent : public Component {
public:
	InputComponent(unsigned long entityId, InputListenerPtr inputListener, ComponentType type=ComponentType::INPUT_COMPONENT) : Component(entityId, type) {
		mInputListener = inputListener;
	}

	void setInputCallback(Input inputEvent, std::function<bool(EventPtr)>& callback);

	void serialize(Serializer& serializer) const override {

	}

protected:
	InputListenerPtr mInputListener;
};

#endif // !__EVENT_H__
