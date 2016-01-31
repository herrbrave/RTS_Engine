#ifndef __GRAPHICS_H__
#define __GRAPHICS_H__

#include"Constants.h"
#include"SDL_Helpers.h"

#include<memory>
#include<unordered_set>

#include<SDL.h>
#include<SDL_ttf.h>

class Graphics;
class Drawable {
public:
	virtual void draw(Graphics* graphicsRef);
};

class Texture {
};

class SDLTexture : public Texture {
public:
	SDLTexture(SDL_Texture* sdlTexture);

	SDL_Texture* getTextureInstrument();

private:
	std::shared_ptr<SDL_Texture> mTexture;
};

class GraphicsConfig {
public:
	GraphicsConfig* setTitle(std::string title);
	GraphicsConfig* setWindowX(int x);
	GraphicsConfig* setWindowY(int y);
	GraphicsConfig* setWindowWidth(int width);
	GraphicsConfig* setWindowHeight(int height);
	GraphicsConfig* fullscreen();
	GraphicsConfig* withOpenGL();
	GraphicsConfig* borderless();
	GraphicsConfig* resizable();
	GraphicsConfig* maximized();

	std::string mTitle = "Game";
	int mWindowX = 0;
	int mWindowY = 0;
	int mWidth = 800;
	int mHeight = 600;
	Uint32 mFlags{ SDL_WINDOW_SHOWN };
};

class Graphics {
public:
	virtual void onBeforeDraw() = 0;
	virtual void renderTexture(Texture* texture, int x, int y) = 0;
	virtual void drawLine(int x0, int y0, int x1, int y1, Uint8 r, Uint8 g, Uint8 b, Uint8 a) = 0;
	virtual void drawSquare(int x, int y, int width, int height, Uint8 r, Uint8 g, Uint8 b, Uint8 a) = 0;
	virtual Texture* createTexture(std::string path, int x, int y, int w, int h) = 0;
	virtual void onAfterDraw() = 0;
};

class SDLGraphics : public Graphics {
public:
	SDLGraphics(GraphicsConfig* graphisConfig);
	virtual void onBeforeDraw();
	virtual void renderTexture(Texture* texture, int x, int y);
	virtual void drawLine(int x0, int y0, int x1, int y1, Uint8 r, Uint8 g, Uint8 b, Uint8 a);
	virtual void drawSquare(int x, int y, int width, int height, Uint8 r, Uint8 g, Uint8 b, Uint8 a);
	virtual Texture* createTexture(std::string path, int x, int y, int w, int h);
	virtual void onAfterDraw();
private:
	std::unique_ptr<SDL_Window, SDL_DELETERS> mWindow{ nullptr };
	std::unique_ptr<SDL_Renderer, SDL_DELETERS> mRenderer{ nullptr };
};

class GraphicsSystem {
public:
	GraphicsSystem(GraphicsConfig* graphisConfig) {
		mGraphicsConfig.reset(graphisConfig);
		mGraphics.reset(new SDLGraphics(mGraphicsConfig.get()));
	}

	void registerDrawable(Drawable* drawable);
	void deregisterDrawable(Drawable* drawable);
	void draw();

private:
	std::shared_ptr<GraphicsConfig> mGraphicsConfig{ nullptr };
	std::unique_ptr<Graphics> mGraphics{ nullptr };
	std::unordered_set<std::shared_ptr<Drawable>> mDrawables;
};

#endif // !__GRAPHICS_H__
