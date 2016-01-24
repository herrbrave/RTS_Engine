#ifndef __SDL_HELPERS_H__
#define __SDL_HELPERS_H__

#include<memory>
#include<string>

#include<SDL.h>
#include<SDL_ttf.h>


struct SDL_DELETERS {
	void operator()(SDL_Window* window) const { SDL_DestroyWindow(window); }
	void operator()(SDL_Renderer* renderer) const { SDL_DestroyRenderer(renderer); }
	void operator()(TTF_Font* font) const { TTF_CloseFont(font); }
	void operator()(SDL_Texture* texture) const { SDL_DestroyTexture(texture); }
};




#endif // !__SDL_HELPERS_H__
