#include"Input.h"

bool InputListener::onEvent(EventType eventType, Event* evt, MouseMovementHandler* mouseMovementHandler) {
	if (eventType == EventType::MOUSE_MOVE) {
		if (mouseMovementHandler->checkForMouseOver(id, *evt->mouseEvent->position.get())) {
			if (mouseState == MouseState::UP) {
				mouseState = MouseState::OVER;
				if (eventCallbacks.find(InputEvent::ON_MOUSE_ENTER) == eventCallbacks.end()) {
					return false;
				}

				auto enterCallback = eventCallbacks.at(InputEvent::ON_MOUSE_ENTER);
				if (enterCallback != nullptr) {
					if (enterCallback(evt)) {
						return true;
					}
				}
			}
			else if (mouseState == MouseState::DOWN) {
				if (eventCallbacks.find(InputEvent::ON_DRAG) == eventCallbacks.end()) {
					return false;
				}

				auto dragCallback = eventCallbacks.at(InputEvent::ON_DRAG);
				if (dragCallback != nullptr) {
					if (dragCallback(evt)) {
						return true;
					}
				}
			}
		}
		else if (mouseState == MouseState::OVER || mouseState == MouseState::DOWN) {
			mouseState = MouseState::UP;
			if (eventCallbacks.find(InputEvent::ON_MOUSE_EXIT) == eventCallbacks.end()) {
				return false;
			}

			auto exitCallback = eventCallbacks.at(InputEvent::ON_MOUSE_EXIT);
			if (exitCallback != nullptr) {
				if (exitCallback(evt)) {
					return true;
				}
			}
		}
	}
	else if (eventType == EventType::MOUSE_BUTTON_DOWN) {
		if (mouseState == MouseState::OVER) {
			mouseState = MouseState::DOWN;
		}
	}
	else if (eventType == EventType::MOUSE_BUTTON_UP) {
		if (mouseState == MouseState::DOWN) {
			mouseState = MouseState::OVER;
			if (eventCallbacks.find(InputEvent::ON_CLICK) == eventCallbacks.end()) {
				return false;
			}

			auto clickCallback = eventCallbacks.at(InputEvent::ON_CLICK);
			if (clickCallback != nullptr) {
				if (clickCallback(evt)) {
					return true;
				}
			}
		}
	}
	else if (eventType == EventType::KEY_DOWN) {
		if (eventCallbacks.find(InputEvent::ON_KEY_DOWN) == eventCallbacks.end()) {
			return false;
		}

		auto keyDownCallback = eventCallbacks.at(InputEvent::ON_KEY_DOWN);
		if (keyDownCallback != nullptr) {
			if (keyDownCallback(evt)) {
				return true;
			}
		}
	}
	else if (eventType == EventType::KEY_UP) {
		if (eventCallbacks.find(InputEvent::ON_KEY_UP) == eventCallbacks.end()) {
			return false;
		}

		auto keyUpCallback = eventCallbacks.at(InputEvent::ON_KEY_UP);
		if (keyUpCallback != nullptr) {
			if (keyUpCallback(evt)) {
				return true;
			}
		}
	}

	return false;
}

void InputComponent::setInputCallback(InputEvent inputEvent, std::function<bool(Event*)>& callback) {
	mInputListener->eventCallbacks.emplace(inputEvent, callback);
}