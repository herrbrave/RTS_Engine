#include"Block.h"

const vector2f& Block::getPosition() {
	return *mPosition;
}

void Block::setPosition(vector2f& position) {
	mPosition->set(&position);
	m_rect_body->x = round(mPosition->x);
	m_rect_body->y = round(mPosition->y);
}

SDL_Rect Block::getRect(std::shared_ptr<Camera> camera) {
	SDL_Rect tranlatedRect{ m_rect_body->x - camera->x, m_rect_body->y - camera->y, m_rect_body->w, m_rect_body->h };
	return tranlatedRect;
}
void Block::draw(SDL_Renderer& renderer, std::shared_ptr<Camera> camera) {
	SDL_Rect tranlatedRect = getRect(camera);
	tranlatedRect.x -= tranlatedRect.w / 2;
	tranlatedRect.y -= tranlatedRect.h / 2;

	if (m_a < 255) {
		SDL_SetRenderDrawBlendMode(&renderer, SDL_BLENDMODE_BLEND);
	}

	SDL_SetRenderDrawColor(&renderer, m_r, m_g, m_b, m_a);

	SDL_RenderFillRect(&renderer, &tranlatedRect);

	SDL_SetRenderDrawBlendMode(&renderer, SDL_BLENDMODE_NONE);

	if (DEBUG) {
		SDL_SetRenderDrawColor(&renderer, 0, 0, 0, 255);
		SDL_RenderDrawLine(&renderer, tranlatedRect.x, tranlatedRect.y, tranlatedRect.x + tranlatedRect.w, tranlatedRect.y);
		SDL_RenderDrawLine(&renderer, tranlatedRect.x + tranlatedRect.w, tranlatedRect.y, tranlatedRect.x + tranlatedRect.w, tranlatedRect.y + tranlatedRect.h);
		SDL_RenderDrawLine(&renderer, tranlatedRect.x + tranlatedRect.w, tranlatedRect.y + tranlatedRect.h, tranlatedRect.x, tranlatedRect.y + tranlatedRect.h);
		SDL_RenderDrawLine(&renderer, tranlatedRect.x, tranlatedRect.y + tranlatedRect.h, tranlatedRect.x, tranlatedRect.y);
	}
}

bool Block::checkCollision(Block& otherBlock) {
	return (m_rect_body->x <= (otherBlock.m_rect_body->x + otherBlock.m_rect_body->w) &&
		(m_rect_body->x + m_rect_body->w) >= otherBlock.m_rect_body->x &&
		m_rect_body->y <= (otherBlock.m_rect_body->y + otherBlock.m_rect_body->h) &&
		(m_rect_body->y + m_rect_body->h) >= otherBlock.m_rect_body->y);
}

bool Block::checkPoint(int x, int y, std::shared_ptr<Camera> camera) {
	SDL_Rect tranlated = getRect(camera);
	tranlated.x -= (tranlated.w / 2);
	tranlated.y -= (tranlated.h / 2);

	return (tranlated.x < x
		&& (tranlated.x + tranlated.w >= x)
		&& tranlated.y < y
		&& (tranlated.y + tranlated.h) >= y
		);
}