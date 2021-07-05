#include"Graphics.h"


void GraphicsConfig::setTitle(const string& title) {
	mTitle = title;
}

void GraphicsConfig::setWindowX(int x) {
	mWindowX = x;
}

void GraphicsConfig::setWindowY(int y) {
	mWindowY = y;
}

void GraphicsConfig::setWindowWidth(int width) {
	mWidth = width;
}

void GraphicsConfig::setWindowHeight(int height) {
	mHeight = height;
}

void GraphicsConfig::fullscreen() {
	mFlags |= SDL_WINDOW_FULLSCREEN;
}

void GraphicsConfig::withOpenGL() {
	mFlags |= SDL_WINDOW_OPENGL;
}

void GraphicsConfig::borderless() {
	mFlags |= SDL_WINDOW_BORDERLESS;
}

void GraphicsConfig::resizable() {
	mFlags |= SDL_WINDOW_RESIZABLE;
}

void GraphicsConfig::maximized() {
	mFlags |= SDL_WINDOW_MAXIMIZED;
}

void GraphicsConfig::setFont(const string& fontPath) {
	mFontPath = fontPath;
}

void GraphicsConfig::setFontSize(int fontSize) {
	mFontSize = fontSize;
}

void Drawable::setDrawDepth(Uint8 depth) {
	mDrawDepth = depth;
}

Uint8 Drawable::getDrawDepth() {
	return mDrawDepth;
}

void Drawable::setColor(Uint8 r, Uint8 g, Uint8 b, Uint8 a) {
	mColor->r = r;
	mColor->g = g;
	mColor->b = b;
	mColor->a = a;
}

void Drawable::setAngle(float angle) {
	this->angle = angle;
}

float Drawable::getAngle() {
	return this->angle;
}

void Drawable::setSize(float width, float height) {
	this->width = width;
	this->height = height;
}

void BlockDrawable::draw(Graphics& graphicsRef, const Vector2f& position) {
	float x = position.x;
	float y = position.y;
	
	graphicsRef.drawSquare(x, y, width, height, mColor->r, mColor->g, mColor->b, mColor->a);
}

void TextureDrawable::draw(Graphics& graphicsRef, const Vector2f& position) {
	graphicsRef.renderTexture(mTexture, position.x, position.y, width, height, this->angle, mColor->r, mColor->g, mColor->b, mColor->a);
}

void TextureDrawable::setSize(float width, float height) {
	Drawable::setSize(width, height);
	mTexture->w = width;
	mTexture->h = height;
}

void TextDrawable::draw(Graphics& graphicsRef, const Vector2f& position) {
	graphicsRef.renderText(this->text, this->font, position.x, position.y, this->mColor->r, this->mColor->g, this->mColor->b, this->mColor->a);
}

SDLGraphics::SDLGraphics(GraphicsConfigPtr graphicsConfig, AssetVendorPtr assetVendor) : Graphics(graphicsConfig, assetVendor) {
	if (graphicsConfig->mFontPath.length() > 0 && TTF_Init() < 0) {
		throw std::exception("Failed to initialize TTF");
	}

	if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) < 0) {
		throw std::exception("Error setting up SDL.");
	}

	if (IMG_Init(IMG_INIT_JPG | IMG_INIT_PNG | IMG_INIT_TIF) < 0) {
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

	// Apply zoom effect
	rect.x *= mGraphicsConfig->mZoom;
	rect.y *= mGraphicsConfig->mZoom;
	rect.w *= mGraphicsConfig->mZoom;
	rect.h *= mGraphicsConfig->mZoom;

	if (a < 255) {
		SDL_SetRenderDrawBlendMode(mRenderer.get(), SDL_BLENDMODE_BLEND);
	}

	SDL_SetRenderDrawColor(mRenderer.get(), r, g, b, a);

	SDL_RenderFillRect(mRenderer.get(), &rect);

	SDL_SetRenderDrawBlendMode(mRenderer.get(), SDL_BLENDMODE_NONE);

	if (__DEBUG__) {
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

void SDLGraphics::renderTexture(TexturePtr texture, float x, float y, float w, float h, float angle, Uint8 r, Uint8 g, Uint8 b, Uint8 a) {

	AssetPtr asset(mAssetVendor->getAsset(texture->assetTag));
	shared_ptr<SDL_Texture> sdlTexture = makeShared(asset->getAsset<SDL_Texture>());
	if (a < 255) {
		SDL_SetTextureAlphaMod(sdlTexture.get(), a);
	}
	SDL_SetRenderDrawColor(mRenderer.get(), r, g, b, a);
	SDL_SetTextureColorMod(sdlTexture.get(), r, g, b);

	SDL_Rect src{ texture->x, texture->y, texture->w, texture->h };
	SDL_Rect dest{ x - (w / 2), y - (h / 2), w, h };
	// if no size is specified
	if (src.w == -1 || src.h == -1) {
		SDL_QueryTexture(sdlTexture.get(), nullptr, nullptr, &src.w, &src.h);
	}
	if (dest.w == -1 || dest.h == -1) {
		dest.x = (x - src.w / 2);
		dest.y = (y - src.h / 2);
		dest.w = src.w;
		dest.h = src.h;
	}

	// apply zoom effect
	dest.x *= mGraphicsConfig->mZoom;
	dest.y *= mGraphicsConfig->mZoom;
	dest.w *= mGraphicsConfig->mZoom;
	dest.h *= mGraphicsConfig->mZoom;

	SDL_RenderCopyEx(mRenderer.get(), sdlTexture.get(), &src, &dest, angle, nullptr, SDL_RendererFlip::SDL_FLIP_NONE);

	if (a < 255) {
		SDL_SetTextureAlphaMod(sdlTexture.get(), 255);
	}

	if (__DEBUG__) {
		SDL_SetRenderDrawColor(mRenderer.get(), 255, 0, 0, 255);
		SDL_RenderDrawLine(mRenderer.get(), dest.x, dest.y, dest.x + dest.w, dest.y);
		SDL_RenderDrawLine(mRenderer.get(), dest.x + dest.w, dest.y, dest.x + dest.w, dest.y + dest.h);
		SDL_RenderDrawLine(mRenderer.get(), dest.x + dest.w, dest.y + dest.h, dest.x, dest.y + dest.h);
		SDL_RenderDrawLine(mRenderer.get(), dest.x, dest.y + dest.h, dest.x, dest.y);
	}
}

void SDLGraphics::renderText(const string& text, const string& font, float x, float y, Uint8 r, Uint8 g, Uint8 b, Uint8 a) {

	AssetPtr asset(mAssetVendor->getAsset(font));
	shared_ptr<TTF_Font> fontPtr = makeShared(asset->getAsset<TTF_Font>());

	SDL_Color color{ r, g, b, a };

	SDL_Surface* sdlSurface = TTF_RenderText_Solid(fontPtr.get(), text.c_str(), color);
	SDL_Texture* sdlTexture = SDL_CreateTextureFromSurface(mRenderer.get(), sdlSurface);

	if (a < 255) {
		SDL_SetTextureAlphaMod(sdlTexture, a);
	}

	int w = -1;
	int h = -1;
	SDL_QueryTexture(sdlTexture, nullptr, nullptr, &w, &h);

	SDL_Rect src{ 0, 0, w, h };
	SDL_Rect dest{ x - (w / 2), y - (h / 2), w, h };

	// apply zoom effect
	dest.x *= mGraphicsConfig->mZoom;
	dest.y *= mGraphicsConfig->mZoom;
	dest.w *= mGraphicsConfig->mZoom;
	dest.h *= mGraphicsConfig->mZoom;

	SDL_RenderCopy(mRenderer.get(), sdlTexture, &src, &dest);

	SDL_DestroyTexture(sdlTexture);
	SDL_FreeSurface(sdlSurface);

	if (__DEBUG__) {
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

AssetPtr SDLGraphics::createTexture(const string& path, const string& assetTag) {
	SDL_Texture* texture = IMG_LoadTexture(mRenderer.get(), path.c_str());
	if (texture == nullptr) {
		SDL_LogError(SDL_LOG_CATEGORY_ERROR, "Failed to load texture at: " + *path.c_str());
	}

	AssetPtr asset(GCC_NEW Asset(VoidPtr(texture, SDL_DELETERS()), path, assetTag));
	return asset;
}

AssetPtr SDLGraphics::createFontAsset(const string& path, const string& assetTag, int fontsize) {
	TTF_Font* font(TTF_OpenFont(path.c_str(), fontsize));

	AssetPtr asset(GCC_NEW Asset(shared_ptr<void>(font, SDL_DELETERS()), path, assetTag));

	return asset;
}

SDLOpenGLGraphics::SDLOpenGLGraphics(GraphicsConfigPtr graphicsConfig, AssetVendorPtr assetVendor) : Graphics(graphicsConfig, assetVendor) {

	if (graphicsConfig->mFontPath.length() > 0 && TTF_Init() < 0) {
		string err("Error setting up TTF : ");
		err.append(SDL_GetError());
		throw std::exception(err.c_str());
	}

	if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) < 0) {
		string err("Error setting up SDL: ");
		err.append(SDL_GetError());
		throw std::exception(err.c_str());
	}

	if (IMG_Init(IMG_INIT_JPG | IMG_INIT_PNG | IMG_INIT_TIF) < 0) {
		string err("Error setting up SDL_image: ");
		err.append(SDL_GetError());
		throw std::exception(err.c_str());
	}

	// Using OpenGL 3.1
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 1);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);

	mWindow.reset(
		SDL_CreateWindow(
			graphicsConfig->mTitle.c_str(),
			graphicsConfig->mWindowX,
			graphicsConfig->mWindowY,
			graphicsConfig->mWidth,
			graphicsConfig->mHeight,
			graphicsConfig->mFlags)
	);

	if (mWindow == nullptr) {
		string err("Error creating window: ");
		err.append(SDL_GetError());
		throw std::exception(err.c_str());
	}

	if (graphicsConfig->mFontPath.length() == 0) {
		return;
	}

	SDL_GLContext context = SDL_GL_CreateContext(mWindow.get());
	mGLContext.reset(
		&context
	);

	if (mGLContext == nullptr) {
		string err("Error creating GL Context: ");
		err.append(SDL_GetError());
		throw std::exception(err.c_str());
	}

	// Use Vsync
	if (SDL_GL_SetSwapInterval(1) < 0) {
		string err("Error turning on Vsync: ");
		err.append(SDL_GetError());
		throw std::exception(err.c_str());
	}

	/*
	// Initialize glew
	glewExperimental = GL_TRUE;

	GLenum result = glewInit();
	if (result != GLEW_OK) {
		string err("Error Initialize Glew: ");
		err.append((char*)glewGetErrorString(result));
		throw std::exception(err.c_str());
	}
	*/

	// Initializing OpenGL
	glEnable(GL_TEXTURE_2D); // Don't forget to enable textures... O.O
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glViewport(0, 0, graphicsConfig->mWidth, graphicsConfig->mHeight);

	// Initialize Projection Matrix
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(0, graphicsConfig->mWidth, graphicsConfig->mHeight, 0, -10, 10);

	GLenum error = glGetError();
	if (GL_NO_ERROR != error) {
		string err("Error setting up project matrix: ");
		err.append((char*)gluErrorString(error));
		throw std::exception(err.c_str());
	}
	
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	error = glGetError();
	if (GL_NO_ERROR != error) {
		string err("Error setting up matrix mode ");
		err.append((char*)gluErrorString(error));
		throw std::exception(err.c_str());
	}
}

void SDLOpenGLGraphics::onBeforeDraw() {
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);
}

void SDLOpenGLGraphics::renderTexture(TexturePtr texture, float x, float y, float w, float h, float angle, Uint8 r, Uint8 g, Uint8 b, Uint8 a) {

	AssetPtr asset(mAssetVendor->getAsset(texture->assetTag));
	shared_ptr<GLTexture> textureId = makeShared(asset->getAsset<GLTexture>());

	GLfloat textureWidth = textureId->w;
	GLfloat textureHeight = textureId->h;

	GLfloat width = (w > 0 ? w : texture->w);
	GLfloat height = (h > 0 ? h : texture->h);

	// Texture Coordinates
	GLfloat x0 = (GLfloat) texture->x / textureWidth;
	GLfloat y0 = (GLfloat) texture->y / textureHeight;
	GLfloat x1 = (GLfloat) (texture->x + texture->w) / textureWidth;
	GLfloat y1 = (GLfloat) (texture->y + texture->h) / textureHeight;

	glPushMatrix();
	glTranslatef(x, y, 0.0f);

	glBindTexture(GL_TEXTURE_2D, textureId->texId);

	glBegin(GL_QUADS);
		glColor4f((GLfloat)r / (GLfloat)255, (GLfloat) g / (GLfloat)255, (GLfloat)b / (GLfloat)255, (GLfloat)a / (GLfloat)255);
		glTexCoord2f(x0, y1);glVertex2f(-width / 2, height / 2);
		glTexCoord2f(x1, y1); glVertex2f(width / 2, height / 2);
		glTexCoord2f(x1, y0); glVertex2f(width / 2, -height / 2);
		glTexCoord2f(x0, y0); glVertex2f(-width / 2, -height / 2);
	glEnd();
	glPopMatrix();
}

void SDLOpenGLGraphics::renderText(const string& text, const string& font, float x, float y, Uint8 r, Uint8 g, Uint8 b, Uint8 a) {
	// no op.
}

void SDLOpenGLGraphics::drawLine(float x0, float y0, float x1, float y1, Uint8 r, Uint8 g, Uint8 b, Uint8 a) {
	glPushMatrix();

	glTranslatef(x0, y0, 0.0f);

	glBegin(GL_LINE);
		glVertex2f(0.0f, 0.0f);
		glVertex2f(x1, y1);
	glEnd();
	glPopMatrix();
}

void SDLOpenGLGraphics::drawSquare(float x, float y, float width, float height, Uint8 r, Uint8 g, Uint8 b, Uint8 a) {
	glPushMatrix();
	glTranslatef(x, y, 0.0f);

	glBegin(GL_QUADS);
		glColor4f((GLfloat)r / (GLfloat)255, (GLfloat)g / (GLfloat)255, (GLfloat)b / (GLfloat)255, (GLfloat)a / (GLfloat)255);
		glVertex2f(0.0f, 0.0f);
		glVertex2f(width, 0.0f);
		glVertex2f(width, height);
		glVertex2f(0.0f, height);
	glEnd();
	glPopMatrix();

	/*
	Vector3f verts[4];
	verts[0] = Vector3f{ -width / 2, height / 2, 0 };
	verts[1] = Vector3f{ width / 2, height / 2, 0 };
	verts[2] = Vector3f{ width / 2, -height / 2, 0 };
	verts[3] = Vector3f{ -width / 2, -height / 2, 0 };

	GLuint vbo;
	glGenBuffers(4, &vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(verts), verts, GL_STATIC_DRAW);

	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);

	glDrawArrays(GL_POINTS, 0, 1);

	glDisableVertexAttribArray(0);
	*/
}

void SDLOpenGLGraphics::onAfterDraw() {
	SDL_GL_SwapWindow(mWindow.get());
}

AssetPtr SDLOpenGLGraphics::createTexture(const std::string& path, const std::string& assetTag) {
	SDL_Surface* surface = IMG_Load(path.c_str());
	if (surface == nullptr) {
		SDL_LogError(SDL_LOG_CATEGORY_ERROR, "Failed to load texture at: " + *path.c_str());
	}

	GLuint texId;
	glGenTextures(1, &texId);

	GLenum textureFormat, internalFormat, textureType;
	if (surface->format->BytesPerPixel == 4) {
		if (surface->format->Rmask == 0x000000ff) {
			textureFormat = GL_RGBA;
			textureType = GL_UNSIGNED_INT_8_8_8_8_REV;
		}
		else {
			textureFormat = GL_BGRA;
			textureType = GL_UNSIGNED_INT_8_8_8_8;
		}
		internalFormat = GL_RGBA8;
	}
	else {
		if (surface->format->Rmask == 0x000000ff) {
			textureFormat = GL_RGB;
			textureType = GL_UNSIGNED_BYTE;
		}
		else {
			textureFormat = GL_BGR;
			textureType = GL_UNSIGNED_BYTE;
		}
		internalFormat = GL_RGB8;
	}

	GLTexture* texture = GCC_NEW GLTexture();
	texture->texId = texId;
	texture->w = surface->w;
	texture->h = surface->h;

	glBindTexture(GL_TEXTURE_2D, texId);

	glTexImage2D(
		GL_TEXTURE_2D, 
		0, 
		internalFormat, 
		surface->w, 
		surface->h, 
		0, 
		textureFormat,
		textureType,
		surface->pixels);
	
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	if (surface) {
		SDL_FreeSurface(surface);
	}

	AssetPtr asset(GCC_NEW Asset(VoidPtr(texture), path, assetTag));
	return asset;

}

AssetPtr SDLOpenGLGraphics::createFontAsset(const string& path, const string& assetTag, int fontsize) {
	TTF_Font* font(TTF_OpenFont(path.c_str(), fontsize));

	AssetPtr asset(GCC_NEW Asset(shared_ptr<void>(font, SDL_DELETERS()), path, assetTag));

	return asset;
}

void DrawableComponent::setColor(Uint8 r, Uint8 g, Uint8 b, Uint8 a) {
	mDrawable->setColor(r, g, b, a);
}

void DrawableComponent::setSize(float width, float height) {
	mDrawable->setSize(width, height);
}

void DrawableComponent::setAngle(float angle) {
	mDrawable->setAngle(angle);
}

float DrawableComponent::getAngle() {
	return mDrawable->getAngle();
}

Uint8 DrawableComponent::getZOrder() {
	return mDrawable->getDrawDepth();
}

void DrawableComponent::setZOrder(Uint8 zOrder) {
	mDrawable->setDrawDepth(zOrder);

	EntityZOrderSetEventData* eventData = GCC_NEW EntityZOrderSetEventData(entityId, SDL_GetTicks());
	EventManager::getInstance().pushEvent(eventData);
}