#include"Graphics.h"


GraphicsConfig* GraphicsConfig::setTitle(std::string title) {
	mTitle = title;
	return this;
}

GraphicsConfig* GraphicsConfig::setWindowX(int x) {
	mWindowX = x;
	return this;
}

GraphicsConfig* GraphicsConfig::setWindowY(int y) {
	mWindowY = y;
	return this;
}

GraphicsConfig* GraphicsConfig::setWindowWidth(int width) {
	mWidth = width;
	return this;
}

GraphicsConfig* GraphicsConfig::setWindowHeight(int height) {
	mHeight = height;
	return this;
}


GraphicsConfig* GraphicsConfig::fullscreen() {
	mFlags |= SDL_WINDOW_FULLSCREEN;
	return this;
}


GraphicsConfig* GraphicsConfig::withOpenGL() {
	mFlags |= SDL_WINDOW_OPENGL;
	return this;
}


GraphicsConfig* GraphicsConfig::borderless() {
	mFlags |= SDL_WINDOW_BORDERLESS;
	return this;
}


GraphicsConfig* GraphicsConfig::resizable() {
	mFlags |= SDL_WINDOW_RESIZABLE;
	return this;
}


GraphicsConfig* GraphicsConfig::maximized() {
	mFlags |= SDL_WINDOW_MAXIMIZED;
	return this;
}


void BlockDrawable::draw(Graphics* graphicsRef, const vector2f* position) {
	float x = position->x;
	float y = position->y;
	
	graphicsRef->drawSquare(x, y, mWidth, mHeight, mColor->r, mColor->g, mColor->b, mColor->a);
}

SDLGraphics::SDLGraphics(GraphicsConfig* graphicsConfig) {
	if (TTF_Init() < 0) {
		throw std::exception("Failed to initialize TTF");
	}

	if (SDL_Init(SDL_INIT_VIDEO) < 0) {
		throw std::exception("Error setting up SDL.");
	}

	mWindow.reset(
		SDL_CreateWindow(
		graphicsConfig->mTitle.c_str(),
		graphicsConfig->mWindowX,
		graphicsConfig->mWindowY,
		graphicsConfig->mWidth,
		graphicsConfig->mHeight,
		graphicsConfig->mFlags)
		);

	mRenderer.reset(
		SDL_CreateRenderer(mWindow.get(), -1, SDL_RENDERER_ACCELERATED)
	);
}

void SDLGraphics::drawSquare(int x, int y, int width, int height, Uint8 r, Uint8 g, Uint8 b, Uint8 a) {
	SDL_Rect rect{ 
		x - (width / 2), 
		y - (height / 2), 
		width, 
		height
	};

	if (a < 255) {
		SDL_SetRenderDrawBlendMode(mRenderer.get(), SDL_BLENDMODE_BLEND);
	}

	SDL_SetRenderDrawColor(mRenderer.get(), r, g, b, a);

	SDL_RenderFillRect(mRenderer.get(), &rect);

	SDL_SetRenderDrawBlendMode(mRenderer.get(), SDL_BLENDMODE_NONE);

	if (DEBUG) {
		SDL_SetRenderDrawColor(mRenderer.get(), 0, 0, 0, 255);
		SDL_RenderDrawLine(mRenderer.get(), rect.x, rect.y, rect.x + rect.w, rect.y);
		SDL_RenderDrawLine(mRenderer.get(), rect.x + rect.w, rect.y, rect.x + rect.w, rect.y + rect.h);
		SDL_RenderDrawLine(mRenderer.get(), rect.x + rect.w, rect.y + rect.h, rect.x, rect.y + rect.h);
		SDL_RenderDrawLine(mRenderer.get(), rect.x, rect.y + rect.h, rect.x, rect.y);
	}
}

void SDLGraphics::drawLine(int x0, int y0, int x1, int y1, Uint8 r, Uint8 g, Uint8 b, Uint8 a) {
	if (a < 255) {
		SDL_SetRenderDrawBlendMode(mRenderer.get(), SDL_BLENDMODE_BLEND);
	}

	SDL_SetRenderDrawColor(mRenderer.get(), r, g, b, a);

	SDL_RenderDrawLine(mRenderer.get(), x0, y0, x1, y1);

	SDL_SetRenderDrawBlendMode(mRenderer.get(), SDL_BLENDMODE_NONE);
}

void SDLGraphics::renderTexture(Texture* texture, int x, int y) {
	SDLTexture* sdlTexture((SDLTexture*)texture);
	int w(0);
	int h(0);
	SDL_QueryTexture(sdlTexture->getTextureInstrument(), nullptr, nullptr, &w, &h);
	SDL_Rect dest{ x - (w / 2), y - (h / 2), 0, 0 };
	SDL_RenderCopy(mRenderer.get(), sdlTexture->getTextureInstrument(), nullptr, &dest);
}


Texture* SDLGraphics::createTexture(std::string path, int x, int y, int w, int h) {
	SDL_Surface* surface = SDL_LoadBMP(path.c_str());
	SDL_Texture* texture = SDL_CreateTextureFromSurface(mRenderer.get(), surface);
	SDL_FreeSurface(surface);

	return new SDLTexture(texture);
}

void SDLGraphics::onBeforeDraw() {
	SDL_SetRenderDrawColor(mRenderer.get(), 0xFF, 0xFF, 0xFF, 0xFF);
	SDL_RenderClear(mRenderer.get());
}

void SDLGraphics::onAfterDraw() {
	SDL_RenderPresent(mRenderer.get());
}

SDLTexture::SDLTexture(SDL_Texture* sdlTexture) {
	mTexture.reset(sdlTexture);
}

SDL_Texture* SDLTexture::getTextureInstrument() {
	return mTexture.get();
}

void GraphicsSystem::registerDrawable(const unsigned long id, Drawable* drawable) {
	std::shared_ptr<Drawable> drawablePtr{ drawable };
	mDrawables.emplace(id, drawablePtr);
}

void  GraphicsSystem::deregisterDrawable(const unsigned long id) {
	mDrawables.erase(mDrawables.find(id));
}

void  GraphicsSystem::draw() {
	mGraphics->onBeforeDraw();

	for (auto drawable : mDrawables) {
		drawable.second->draw(mGraphics.get(), mPhysicsSystem->getBody(drawable.first)->getPosition());
	}

	mGraphics->onAfterDraw();
}