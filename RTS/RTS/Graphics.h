#ifndef __GRAPHICS_H__
#define __GRAPHICS_H__

#include"Camera.h"
#include"Constants.h"
#include"Physics.h"
#include"SDL_Helpers.h"

#include<memory>
#include<unordered_map>

#include<SDL.h>
#include<SDL_ttf.h>

class Graphics;
class Drawable {
public:
	Drawable(float width, float height) {
		mWidth = width;
		mHeight = height;
	}

	virtual void draw(Graphics* graphicsRef, const vector2f* position) = 0;

protected:
	float mWidth;
	float mHeight;
};

class BlockDrawable : public Drawable {
public:
	BlockDrawable(float width, float height, Uint8 r, Uint8 g, Uint8 b, Uint8 a) : Drawable(width, height) {
		mColor.reset(new SDL_Color{r, g, b, a});
	}

	void draw(Graphics* graphicsRef, const vector2f* position) override;

private:
	std::unique_ptr<SDL_Color> mColor;
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
	virtual void renderTexture(Texture* texture, float x, float y) = 0;
	virtual void drawLine(float x0, float y0, float x1, float y1, Uint8 r, Uint8 g, Uint8 b, Uint8 a) = 0;
	virtual void drawSquare(float x, float y, float width, float height, Uint8 r, Uint8 g, Uint8 b, Uint8 a) = 0;
	virtual Texture* createTexture(std::string path, int x, int y, int w, int h) = 0;
	virtual void onAfterDraw() = 0;
};

class SDLGraphics : public Graphics {
public:
	SDLGraphics(GraphicsConfig* graphisConfig);
	virtual void onBeforeDraw();
	virtual void renderTexture(Texture* texture, float x, float y);
	virtual void drawLine(float x0, float y0, float x1, float y1, Uint8 r, Uint8 g, Uint8 b, Uint8 a);
	virtual void drawSquare(float x, float y, float width, float height, Uint8 r, Uint8 g, Uint8 b, Uint8 a);
	virtual Texture* createTexture(std::string path, int x, int y, int w, int h);
	virtual void onAfterDraw();
private:
	std::unique_ptr<SDL_Window, SDL_DELETERS> mWindow{ nullptr };
	std::unique_ptr<SDL_Renderer, SDL_DELETERS> mRenderer{ nullptr };
};

class GraphicsSystem {
public:
	GraphicsSystem(GraphicsConfig* graphisConfig, PhysicsSystem* physicsSystem) {
		mPhysicsSystem.reset(physicsSystem);
		mGraphicsConfig.reset(graphisConfig);
		mGraphics.reset(new SDLGraphics(mGraphicsConfig.get()));
		mCamera.reset(new Camera());
		mCamera->position.reset(new vector2f(0, 0));
		mCamera->width = mGraphicsConfig->mWidth;
		mCamera->height = mGraphicsConfig->mHeight;
	}

	void registerDrawable(const unsigned long, Drawable* drawable);
	void deregisterDrawable(const unsigned long);
	void draw();

	Camera* getCamera();
private:
	std::shared_ptr<PhysicsSystem> mPhysicsSystem{ nullptr };
	std::shared_ptr<GraphicsConfig> mGraphicsConfig{ nullptr };
	std::unique_ptr<Graphics> mGraphics{ nullptr };
	std::unique_ptr<Camera> mCamera;
	std::unordered_map<unsigned long, std::shared_ptr<Drawable>> mDrawables;
};

#endif // !__GRAPHICS_H__
