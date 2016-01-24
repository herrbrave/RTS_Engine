#ifndef __GAME_H__
#define __GAME_H__

#include<SDL.h>

class Game {
protected:
	bool mIsPlaying{ true };

	virtual void setup() = 0;
	virtual void handleEvents() = 0;
	virtual void update() = 0;
	virtual void draw() = 0;
	virtual void delay(Uint32 frameTime) = 0;
	virtual void teardown() = 0;

public:
	void play() {
		setup();

		while (mIsPlaying) {
			Uint32 start(SDL_GetTicks());

			handleEvents();
			update();
			draw();

			Uint32 end(SDL_GetTicks());
			delay(end - start);
		}

		teardown();
	}
};

#endif