#ifndef __GAME_H__
#define __GAME_H__

#include <iostream>
#include <chrono>

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

			std::cout << "########################## START FRAME ##########################" << std::endl;
			auto sf = std::chrono::high_resolution_clock::now();
			auto s = std::chrono::high_resolution_clock::now();
			handleEvents();

			auto e = std::chrono::high_resolution_clock::now();
			auto res = std::chrono::duration_cast<std::chrono::milliseconds>(e - s);
			std::cout << "Handle Events: " << res.count() << std::endl;

			s = std::chrono::high_resolution_clock::now();
			update();

			e = std::chrono::high_resolution_clock::now();
			res = std::chrono::duration_cast<std::chrono::milliseconds>(e - s);
			std::cout << "Update: " << res.count() << std::endl;

			s = std::chrono::high_resolution_clock::now();
			draw();

			e = std::chrono::high_resolution_clock::now();
			res = std::chrono::duration_cast<std::chrono::milliseconds>(e - s);
			std::cout << "Draw: " << res.count() << std::endl;

			res = std::chrono::duration_cast<std::chrono::milliseconds>(e - sf);
			std::cout << "Total Frame Time: " << res.count() << std::endl;
			std::cout << "###############################################################" << std::endl;

			Uint32 end(SDL_GetTicks());
			delay(end - start);
		}

		teardown();
	}
};

#endif