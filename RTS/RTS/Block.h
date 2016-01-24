#ifndef __BLOCK_H__
#define __BLOCK_H__

#include <memory>
#include<SDL.h>

#include"Camera.h"
#include"Constants.h"
#include"vector2f.h"


class Block {
public:
	std::unique_ptr<SDL_Rect> m_rect_body;

	/* color */
	Uint8 m_r;
	Uint8 m_g;
	Uint8 m_b;
	Uint8 m_a;

	Block() : Block(100, 100, 100, 100, 255, 255, 255) {}

	Block(int x, int y, int w, int h, Uint8 r, Uint8 g, Uint8 b) {
		m_rect_body = std::unique_ptr<SDL_Rect>(new SDL_Rect());
		m_rect_body->x = x;
		m_rect_body->y = y;
		m_rect_body->w = w;
		m_rect_body->h = h;

		mPosition = std::shared_ptr<vector2f>(new vector2f(x, y));

		m_r = r;
		m_g = g;
		m_b = b;
		m_a = 255;
	}

	const vector2f& getPosition();

	void setPosition(vector2f& position);

	SDL_Rect getRect(std::shared_ptr<Camera>);

	void draw(SDL_Renderer& renderer, std::shared_ptr<Camera>);

	bool checkCollision(Block& otherBlock);

	bool checkPoint(int x, int y, std::shared_ptr<Camera> camera);

private:
	std::shared_ptr<vector2f> mPosition;
};

#endif