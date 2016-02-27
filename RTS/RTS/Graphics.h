#ifndef __GRAPHICS_H__
#define __GRAPHICS_H__

#include"Asset.h"
#include"Camera.h"
#include"Constants.h"
#include"Physics.h"
#include"SDL_Helpers.h"
#include"Texture.h"

#include<memory>
#include<unordered_map>

#include<SDL.h>
#include<SDL_ttf.h>
#include<SDL_image.h>

class Graphics;
class Drawable {
public:
	float width;
	float height;

	Drawable(float width, float height) {
		this->width = width;
		this->height = height;
		mColor.reset(new SDL_Color{ 255, 255, 255, 255 });
	}

	virtual void draw(Graphics* graphicsRef, const vector2f* position) = 0;

	void setColor(Uint8 r, Uint8 g, Uint8 b, Uint8 a);

	virtual void setSize(float width, float height);

	virtual void serialize(Serializer& serializer) const {
		serializer.writer.StartObject();

		serializer.writer.String("width");
		serializer.writer.Uint(width);

		serializer.writer.String("height");
		serializer.writer.Uint(height);

		serializer.writer.String("r");
		serializer.writer.Uint(mColor->r);

		serializer.writer.String("g");
		serializer.writer.Uint(mColor->g);

		serializer.writer.String("b");
		serializer.writer.Uint(mColor->b);

		serializer.writer.String("a");
		serializer.writer.Uint(mColor->a);

		onSerialize(serializer);

		serializer.writer.EndObject();
	}

protected:
	std::unique_ptr<SDL_Color> mColor;
	virtual void onSerialize(Serializer& serializer) const = 0;
};

class BlockDrawable : public Drawable {
public:
	BlockDrawable(float width, float height, Uint8 r, Uint8 g, Uint8 b, Uint8 a) : Drawable(width, height) {
		mColor->r = r;
		mColor->g = g;
		mColor->b = b;
		mColor->a = a;
	}

	void draw(Graphics* graphicsRef, const vector2f* position) override;

	void setColor(Uint8 r, Uint8 g, Uint8 b, Uint8 a);
protected:

	virtual void onSerialize(Serializer& serializer) const {
		serializer.writer.String("drawableType");
		serializer.writer.String("BlockDrawable");
	}
};

class TextureDrawable : public Drawable {
public:
	TextureDrawable(Texture* texture) : Drawable(texture->w, texture->h) {
		mTexture.reset(texture);
	}

	void draw(Graphics* graphicsRef, const vector2f* position) override;

	void setSize(float width, float height) override;

protected:
	virtual void onSerialize(Serializer& serializer) const {
		serializer.writer.String("drawableType");
		serializer.writer.String("TextureDrawable");

		serializer.writer.String("mTexture");
		mTexture->serialize(serializer);
	}

private:
	std::unique_ptr<Texture> mTexture{ nullptr };
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

	~GraphicsConfig() {
	}

	std::string mTitle;
	int mWindowX = 0;
	int mWindowY = 0;
	int mWidth = 800;
	int mHeight = 600;
	Uint32 mFlags{ SDL_WINDOW_SHOWN };
};

class Graphics {
public:
	Graphics(AssetSystem* assetSystem) {
		mAssetSystem = assetSystem;
	}

	virtual void onBeforeDraw() = 0;
	virtual void renderTexture(Texture* texture, float x, float y, Uint8 r, Uint8 g, Uint8 b, Uint8 a) = 0;
	virtual void drawLine(float x0, float y0, float x1, float y1, Uint8 r, Uint8 g, Uint8 b, Uint8 a) = 0;
	virtual void drawSquare(float x, float y, float width, float height, Uint8 r, Uint8 g, Uint8 b, Uint8 a) = 0;
	virtual void onAfterDraw() = 0;

	virtual Asset* createTexture(const std::string& path, const std::string& assetTag) = 0;

protected:
	AssetSystem* mAssetSystem;
};

class SDLGraphics : public Graphics {
public:
	SDLGraphics(AssetSystem* assetSystem, GraphicsConfig* graphisConfig);
	void onBeforeDraw() override;
	void renderTexture(Texture* texture, float x, float y, Uint8 r, Uint8 g, Uint8 b, Uint8 a) override;
	void drawLine(float x0, float y0, float x1, float y1, Uint8 r, Uint8 g, Uint8 b, Uint8 a) override;
	void drawSquare(float x, float y, float width, float height, Uint8 r, Uint8 g, Uint8 b, Uint8 a) override;
	void onAfterDraw() override;

	SDL_Renderer* getRenderer();
	Asset* createTexture(const std::string& path, const std::string& assetTag) override;
private:

	std::unique_ptr<SDL_Window, SDL_DELETERS> mWindow{ nullptr };
	std::unique_ptr<SDL_Renderer, SDL_DELETERS> mRenderer{ nullptr };
};

class GraphicsSystem {
public:
	GraphicsSystem(AssetSystem* assetSystem, GraphicsConfig* graphisConfig, PhysicsSystem* physicsSystem) {
		mAssetSystem = assetSystem;
		mPhysicsSystem = physicsSystem;
		mGraphicsConfig.reset(std::move(graphisConfig));
		mGraphics.reset(new SDLGraphics(mAssetSystem, mGraphicsConfig.get()));
		mCamera.reset(new Camera());
		mCamera->position.reset(new vector2f(0, 0));
		mCamera->width = mGraphicsConfig->mWidth;
		mCamera->height = mGraphicsConfig->mHeight;
	}

	void registerDrawable(const unsigned long, Drawable* drawable);
	void deregisterDrawable(const unsigned long);
	void draw();

	Camera* getCamera();

	void addTexture(const std::string& path, const std::string& assetTag);
private:
	AssetSystem* mAssetSystem{ nullptr };
	PhysicsSystem* mPhysicsSystem{ nullptr };
	std::unique_ptr<GraphicsConfig> mGraphicsConfig{ nullptr };
	std::unique_ptr<Graphics> mGraphics{ nullptr };
	std::unique_ptr<Camera> mCamera;
	std::unordered_map<unsigned long, std::shared_ptr<Drawable>> mDrawables;
};

const static unsigned long DRAWABLE_COMPONENT_TYPE = sComponentId++;

class DrawableComponent : public Component {
public:
	DrawableComponent(unsigned long entityId, Drawable* drawable) : Component(entityId, DRAWABLE_COMPONENT_TYPE) {
		mDrawable = drawable;
	}

	void setColor(Uint8 r, Uint8 g, Uint8 b, Uint8 a);
	void setSize(float width, float height);

	void serialize(Serializer& serializer) const override {
		serializer.writer.StartObject();

		serializer.writer.String("componentId");
		serializer.writer.Uint64(DRAWABLE_COMPONENT_TYPE);
		serializer.writer.String("mDrawable");
		mDrawable->serialize(serializer);

		serializer.writer.EndObject();
	}
private:
	Drawable* mDrawable;
};

#endif // !__GRAPHICS_H__
