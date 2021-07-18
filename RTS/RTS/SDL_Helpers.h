#ifndef __SDL_HELPERS_H__
#define __SDL_HELPERS_H__

#include<memory>
#include<string>

#include<SDL.h>
#include<SDL_mixer.h>
#include<SDL_ttf.h>


struct SDL_DELETERS {
	void operator()(SDL_Window* window) const { SDL_DestroyWindow(window); }
	void operator()(SDL_Renderer* renderer) const { SDL_DestroyRenderer(renderer); }
	void operator()(SDL_GLContext* context) const { SDL_GL_DeleteContext(context); }
	void operator()(TTF_Font* font) const { TTF_CloseFont(font); }
	void operator()(SDL_Texture* texture) const { SDL_DestroyTexture(texture); }
	void operator()(Mix_Chunk* mix) const { Mix_FreeChunk(mix); }
	void operator()(Mix_Music* mix) const { Mix_FreeMusic(mix); }
};

typedef std::shared_ptr<SDL_Texture> SDL_TexturePtr;

#endif // !__SDL_HELPERS_H__
