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

GraphicsConfig* GraphicsConfig::setFont(const std::string& fontPath) {
	mFontPath = fontPath;
	return this;
}

GraphicsConfig* GraphicsConfig::setFontSize(int fontSize) {
	mFontSize = fontSize;
	return this;
}

void Drawable::setColor(Uint8 r, Uint8 g, Uint8 b, Uint8 a) {
	mColor->r = r;
	mColor->g = g;
	mColor->b = b;
	mColor->a = a;
}

void Drawable::setSize(float width, float height) {
	this->width = width;
	this->height = height;
}

void BlockDrawable::draw(Graphics* graphicsRef, const vector2f* position) {
	float x = position->x;
	float y = position->y;
	
	graphicsRef->drawSquare(x, y, width, height, mColor->r, mColor->g, mColor->b, mColor->a);
}

void TextureDrawable::draw(Graphics* graphicsRef, const vector2f* position) {
	graphicsRef->renderTexture(mTexture.get(), position->x, position->y, width, height, mColor->r, mColor->g, mColor->b, mColor->a);
}

void TextureDrawable::setSize(float width, float height) {
	Drawable::setSize(width, height);
	mTexture->w = width;
	mTexture->h = height;
}

SDLGraphics::SDLGraphics(GraphicsConfig* graphicsConfig, AssetVendor* assetVendor) : Graphics(assetVendor) {
	if (graphicsConfig->mFontPath.length() > 0 && TTF_Init() < 0) {
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

	if (graphicsConfig->mFontPath.length() == 0) {
		return;
	}

	TTF_Font* font(TTF_OpenFont(graphicsConfig->mFontPath.c_str(), graphicsConfig->mFontSize));
	mFont.reset(font);
}

void SDLGraphics::drawSquare(float x, float y, float width, float height, Uint8 r, Uint8 g, Uint8 b, Uint8 a) {
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
		SDL_SetRenderDrawColor(mRenderer.get(), 255, 0, 0, 255);
		SDL_RenderDrawLine(mRenderer.get(), rect.x, rect.y, rect.x + rect.w, rect.y);
		SDL_RenderDrawLine(mRenderer.get(), rect.x + rect.w, rect.y, rect.x + rect.w, rect.y + rect.h);
		SDL_RenderDrawLine(mRenderer.get(), rect.x + rect.w, rect.y + rect.h, rect.x, rect.y + rect.h);
		SDL_RenderDrawLine(mRenderer.get(), rect.x, rect.y + rect.h, rect.x, rect.y);
	}
}

void SDLGraphics::drawLine(float x0, float y0, float x1, float y1, Uint8 r, Uint8 g, Uint8 b, Uint8 a) {
	if (a < 255) {
		SDL_SetRenderDrawBlendMode(mRenderer.get(), SDL_BLENDMODE_BLEND);
	}

	SDL_SetRenderDrawColor(mRenderer.get(), r, g, b, a);

	SDL_RenderDrawLine(mRenderer.get(), x0, y0, x1, y1);

	SDL_SetRenderDrawBlendMode(mRenderer.get(), SDL_BLENDMODE_NONE);
}

void SDLGraphics::renderTexture(Texture* texture, float x, float y, float w, float h, Uint8 r, Uint8 g, Uint8 b, Uint8 a) {

	Asset* asset = mAssetVendor->getAsset(texture->assetTag);
	SDL_Texture* sdlTexture = reinterpret_cast<SDL_Texture*>(asset->getAsset());
	if (a < 255) {
		SDL_SetTextureAlphaMod(sdlTexture, a);
	}
	SDL_SetRenderDrawColor(mRenderer.get(), r, g, b, a);

	SDL_Rect src{ texture->x, texture->y, texture->w, texture->h };
	SDL_Rect dest{ x - (w / 2), y - (h / 2), w, h };
	// if no size is specified
	if (src.w == -1 || src.h == -1) {
		SDL_QueryTexture(sdlTexture, nullptr, nullptr, &src.w, &src.h);
	}
	if (dest.w == -1 || dest.h == -1) {
		dest.x = (x - src.w / 2);
		dest.y = (y - src.h / 2);
		dest.w = src.w;
		dest.h = src.h;
	}

	SDL_RenderCopy(mRenderer.get(), sdlTexture, &src, &dest);

	if (a < 255) {
		SDL_SetTextureAlphaMod(sdlTexture, 255);
	}

	if (DEBUG) {
		SDL_SetRenderDrawColor(mRenderer.get(), 255, 0, 0, 255);
		SDL_RenderDrawLine(mRenderer.get(), dest.x, dest.y, dest.x + dest.w, dest.y);
		SDL_RenderDrawLine(mRenderer.get(), dest.x + dest.w, dest.y, dest.x + dest.w, dest.y + dest.h);
		SDL_RenderDrawLine(mRenderer.get(), dest.x + dest.w, dest.y + dest.h, dest.x, dest.y + dest.h);
		SDL_RenderDrawLine(mRenderer.get(), dest.x, dest.y + dest.h, dest.x, dest.y);
	}
}

void SDLGraphics::onBeforeDraw() {
	SDL_SetRenderDrawColor(mRenderer.get(), 0x00, 0x00, 0x00, 0x00);
	SDL_RenderClear(mRenderer.get());
}

void SDLGraphics::onAfterDraw() {
	SDL_RenderPresent(mRenderer.get());
}

SDL_Renderer* SDLGraphics::getRenderer() {
	return mRenderer.get();
}

Asset* SDLGraphics::createTexture(const std::string& path, const std::string& assetTag) {
	SDL_Texture* texture = IMG_LoadTexture(mRenderer.get(), path.c_str());
	if (texture == nullptr) {
		SDL_LogError(SDL_LOG_CATEGORY_ERROR, "Failed to load texture at: " + *path.c_str());
	}

	Asset* asset = new Asset(texture, path, assetTag, std::function<void(void*)>([](void* deleteMe){SDL_DestroyTexture(reinterpret_cast<SDL_Texture*>(deleteMe)); }));
	return asset;
}

Asset* SDLGraphics::createTextAsset(const std::string& text, const std::string& assetTag, Uint8 r, Uint8 g, Uint8 b, Uint8 a) {
	SDL_Color textColor{ r, g, b, a };
	SDL_Surface* surface = TTF_RenderText_Solid(mFont.get(), text.c_str(), textColor);
	SDL_Texture* sdlTexture = SDL_CreateTextureFromSurface(mRenderer.get(), surface);
	SDL_FreeSurface(surface);

	Asset* asset = new Asset(sdlTexture, "", assetTag, std::function<void(void*)>([](void* deleteMe){SDL_DestroyTexture(reinterpret_cast<SDL_Texture*>(deleteMe)); }));

	return asset;
}

void DrawableComponent::setColor(Uint8 r, Uint8 g, Uint8 b, Uint8 a) {
	mDrawable->setColor(r, g, b, a);
}

void DrawableComponent::setSize(float width, float height) {
	mDrawable->setSize(width, height);
}