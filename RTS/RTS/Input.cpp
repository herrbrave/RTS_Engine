#include"Input.h"

bool InputListener::onEvent(InputEventType eventType, EventPtr evt, MouseMovementHandlerPtr mouseMovementHandler) {
	if (eventType == InputEventType::MOUSE_MOVE) {
		if (mouseMovementHandler->checkForMouseOver(id, *evt->mouseEvent->position.get())) {
			if (mouseState == MouseState::UP) {
				mouseState = MouseState::OVER;
				if (eventCallbacks.find(Input::ON_MOUSE_ENTER) == eventCallbacks.end()) {
					return false;
				}

				auto enterCallback = eventCallbacks.at(Input::ON_MOUSE_ENTER);
				if (enterCallback != nullptr) {
					if (enterCallback(evt)) {
						return true;
					}
				}
			}
			else if (mouseState == MouseState::DOWN) {
				if (eventCallbacks.find(Input::ON_DRAG) == eventCallbacks.end()) {
					return false;
				}

				auto dragCallback = eventCallbacks.at(Input::ON_DRAG);
				if (dragCallback != nullptr) {
					if (dragCallback(evt)) {
						return true;
					}
				}
			}
		}
		else if (mouseState == MouseState::OVER || mouseState == MouseState::DOWN) {
			mouseState = MouseState::UP;
			if (eventCallbacks.find(Input::ON_MOUSE_EXIT) == eventCallbacks.end()) {
				return false;
			}

			auto exitCallback = eventCallbacks.at(Input::ON_MOUSE_EXIT);
			if (exitCallback != nullptr) {
				if (exitCallback(evt)) {
					return true;
				}
			}
		}

		if (eventCallbacks.find(Input::ON_MOUSE_MOVE) != eventCallbacks.end()) {
			auto mouseMove = eventCallbacks.at(Input::ON_MOUSE_MOVE);
			mouseMove(evt);
		}
	}
	else if (eventType == InputEventType::MOUSE_BUTTON_DOWN) {
		if (mouseState == MouseState::OVER) {
			mouseState = MouseState::DOWN;
			if (eventCallbacks.find(Input::ON_MOUSE_DOWN) == eventCallbacks.end()) {
				return false;
			}
			
			auto clickCallback = eventCallbacks.at(Input::ON_MOUSE_DOWN);
			if (clickCallback != nullptr) {
				if (clickCallback(evt)) {
					return true;
				}
			}
		}
	}
	else if (eventType == InputEventType::MOUSE_BUTTON_UP) {
		if (mouseState == MouseState::DOWN) {
			mouseState = MouseState::OVER;
			if (eventCallbacks.find(Input::ON_CLICK) == eventCallbacks.end()) {
				return false;
			}

			auto clickCallback = eventCallbacks.at(Input::ON_CLICK);
			if (clickCallback != nullptr) {
				if (clickCallback(evt)) {
					return true;
				}
			}
		}
	}
	else if (eventType == InputEventType::KEY_DOWN) {
		if (eventCallbacks.find(Input::ON_KEY_DOWN) == eventCallbacks.end()) {
			return false;
		}

		auto keyDownCallback = eventCallbacks.at(Input::ON_KEY_DOWN);
		if (keyDownCallback != nullptr) {
			if (keyDownCallback(evt)) {
				return true;
			}
		}
	}
	else if (eventType == InputEventType::KEY_UP) {
		if (eventCallbacks.find(Input::ON_KEY_UP) == eventCallbacks.end()) {
			return false;
		}

		auto keyUpCallback = eventCallbacks.at(Input::ON_KEY_UP);
		if (keyUpCallback != nullptr) {
			if (keyUpCallback(evt)) {
				return true;
			}
		}
	}

	return false;
}

void InputComponent::setInputCallback(Input inputEvent, std::function<bool(EventPtr)>& callback) {
	mInputListener->eventCallbacks.emplace(inputEvent, callback);
}