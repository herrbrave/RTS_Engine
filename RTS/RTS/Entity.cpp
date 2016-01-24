#include"Entity.h"

Entity::Entity(int x, int y, int width, int height, Uint8 r, Uint8 g, Uint8 b, std::shared_ptr<Camera> camera) {
	mBlock.reset(new Block( x, y, width, height, r, g, b));
	mCamera = camera;
}

void Entity::update() {
	if (mStateQueue.size() <= 0) {
		return;
	}

	auto currentState = mStateQueue[0];
	if (!currentState->hasBegun()) {
		currentState->begin();
	}

	if (currentState->isEnded()) {
		mStateQueue.erase(std::begin(mStateQueue));
		if (mStateQueue.size() > 0) {
			currentState = mStateQueue[0];
			currentState->begin();
		}
		else {
			currentState = nullptr;
		}
	}

	if (currentState != nullptr) {
		currentState->update();
	}
}

void Entity::setLocation(vector2f& location) {
	mBlock->setPosition(location);
}

const vector2f& Entity::getLocation() {
	return mBlock->getPosition();
}

void Entity::draw(std::shared_ptr<SDL_Renderer> renderer) {
	mBlock->draw(*renderer.get(), mCamera);
}

void Entity::pushState(std::shared_ptr<State> state) {
	mStateQueue.push_back(state);
}

bool Entity::checkCollision(Body& body) {
	Extent targetExtent = body.getExtent();
	Extent localExtent = getExtent();

	return (localExtent.x0 <= targetExtent.x1 && localExtent.x1 >= targetExtent.x0) &&
		(localExtent.y0 <= targetExtent.y1 && localExtent.y1 >= targetExtent.y0);
}

bool Entity::checkPoint(vector2f& point) {
	return mBlock->checkPoint(point[0], point[1], mCamera);
}

Extent Entity::getExtent() {
	return Extent{ 
		mBlock->m_rect_body->x - (mBlock->m_rect_body->w / 2), 
		mBlock->m_rect_body->y - (mBlock->m_rect_body->h / 2),
		mBlock->m_rect_body->x + (mBlock->m_rect_body->w / 2),
		mBlock->m_rect_body->y + (mBlock->m_rect_body->h / 2),
	};
}