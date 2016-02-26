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

void Drawable::setColor(Uint8 r, Uint8 g, Uint8 b, Uint8 a) {
	mColor->r = r;
	mColor->g = g;
	mColor->b = b;
	mColor->a = a;
}

void BlockDrawable::draw(Graphics* graphicsRef, const vector2f* position) {
	float x = position->x;
	float y = position->y;
	
	graphicsRef->drawSquare(x, y, width, height, mColor->r, mColor->g, mColor->b, mColor->a);
}

void TextureDrawable::draw(Graphics* graphicsRef, const vector2f* position) {
	graphicsRef->renderTexture(mTexture.get(), position->x, position->y, mColor->r, mColor->g, mColor->b, mColor->a);
}

SDLGraphics::SDLGraphics(AssetSystem* assetSystem, GraphicsConfig* graphicsConfig) : Graphics(assetSystem) {
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
		SDL_SetRenderDrawColor(mRenderer.get(), 0, 0, 0, 255);
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

void SDLGraphics::renderTexture(Texture* texture, float x, float y, Uint8 r, Uint8 g, Uint8 b, Uint8 a) {

	Asset* asset = mAssetSystem->getAsset(texture->assetTag);
	SDL_Texture* sdlTexture = reinterpret_cast<SDL_Texture*>(asset->getAsset());
	if (a < 255) {
		SDL_SetTextureAlphaMod(sdlTexture, a);
	}
	SDL_SetRenderDrawColor(mRenderer.get(), r, g, b, a);

	SDL_Rect src{ texture->x, texture->y, texture->w, texture->h };
	SDL_Rect dest{ x - (texture->w / 2), y - (texture->h / 2), texture->w, texture->h };
	SDL_RenderCopy(mRenderer.get(), sdlTexture, &src, &dest);

	if (a < 255) {
		SDL_SetTextureAlphaMod(sdlTexture, 255);
	}

	if (DEBUG) {
		SDL_SetRenderDrawColor(mRenderer.get(), 0, 0, 0, 255);
		SDL_RenderDrawLine(mRenderer.get(), dest.x, dest.y, dest.x + dest.w, dest.y);
		SDL_RenderDrawLine(mRenderer.get(), dest.x + dest.w, dest.y, dest.x + dest.w, dest.y + dest.h);
		SDL_RenderDrawLine(mRenderer.get(), dest.x + dest.w, dest.y + dest.h, dest.x, dest.y + dest.h);
		SDL_RenderDrawLine(mRenderer.get(), dest.x, dest.y + dest.h, dest.x, dest.y);
	}
}

void SDLGraphics::onBeforeDraw() {
	SDL_SetRenderDrawColor(mRenderer.get(), 0xFF, 0xFF, 0xFF, 0xFF);
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

	Asset* asset = new Asset(texture, assetTag, std::function<void(void*)>([](void* deleteMe){SDL_DestroyTexture(reinterpret_cast<SDL_Texture*>(deleteMe)); }));
	return asset;
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
		vector2f position(*mPhysicsSystem->getBody(drawable.first)->getPosition());
		translateToCamera(&position, mCamera.get());

		drawable.second->draw(mGraphics.get(), &position);
	}

	mGraphics->onAfterDraw();
}

Camera* GraphicsSystem::getCamera() {
	return mCamera.get();
}

void GraphicsSystem::addTexture(const std::string& path, const std::string& assetTag) {
	if (mAssetSystem->contains(assetTag)) {
		SDL_Log("Asset already loaded.");
	}

	mAssetSystem->registerAsset(mGraphics->createTexture(path, assetTag));
}

void DrawableComponent::setColor(Uint8 r, Uint8 g, Uint8 b, Uint8 a) {
	mDrawable->setColor(r, g, b, a);
}

void DrawableComponent::setSize(float width, float height) {
	mDrawable->width = width;
	mDrawable->height = height;
}