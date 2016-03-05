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
	bool isUi{ false };

	Drawable(float width, float height) {
		this->width = width;
		this->height = height;
		mColor.reset(new SDL_Color{ 255, 255, 255, 255 });
	}

	Drawable(const rapidjson::Value& root) {
		width = root["width"].GetDouble();
		height = root["height"].GetDouble();
		mColor.reset(new SDL_Color{ 
			root["r"].GetUint(), 
			root["g"].GetUint(),
			root["b"].GetUint(),
			root["a"].GetUint()
		});
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

	BlockDrawable(const rapidjson::Value& root) : Drawable(root) {}

	void draw(Graphics* graphicsRef, const vector2f* position) override;
protected:

	virtual void onSerialize(Serializer& serializer) const {
		serializer.writer.StartObject();

		serializer.writer.String("drawableType");
		serializer.writer.String("BlockDrawable");

		serializer.writer.String("r");
		serializer.writer.Uint(mColor->r);

		serializer.writer.String("g");
		serializer.writer.Uint(mColor->g);

		serializer.writer.String("b");
		serializer.writer.Uint(mColor->b);

		serializer.writer.String("a");
		serializer.writer.Uint(mColor->a);

		serializer.writer.EndObject();
	}
};

class TextureDrawable : public Drawable {
public:
	TextureDrawable(Texture* texture) : Drawable(texture->w, texture->h) {
		mTexture.reset(texture);
	}

	TextureDrawable(const rapidjson::Value& root) : Drawable(0, 0) {
		const rapidjson::Value& texture = root["mTexture"];
		mTexture.reset(new Texture(texture));
	}

	void draw(Graphics* graphicsRef, const vector2f* position) override;

	void setSize(float width, float height) override;

protected:
	virtual void onSerialize(Serializer& serializer) const {
		serializer.writer.StartObject();

		serializer.writer.String("drawableType");
		serializer.writer.String("TextureDrawable");

		serializer.writer.String("mTexture");
		mTexture->serialize(serializer);

		serializer.writer.EndObject();
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
	GraphicsConfig* setFont(const std::string& fontPath);
	GraphicsConfig* setFontSize(int fontSize);

	~GraphicsConfig() {
	}

	std::string mTitle;
	std::string mFontPath;
	int mWindowX = 0;
	int mWindowY = 0;
	int mWidth = 800;
	int mHeight = 600;
	int mFontSize = 12;
	Uint32 mFlags{ SDL_WINDOW_SHOWN };
};

class Graphics {
public:
	Graphics(AssetVendor* assetVendor) { mAssetVendor.reset(assetVendor); }
	virtual void onBeforeDraw() = 0;
	virtual void renderTexture(Texture* texture, float x, float y, float w, float h, Uint8 r, Uint8 g, Uint8 b, Uint8 a) = 0;
	virtual void drawLine(float x0, float y0, float x1, float y1, Uint8 r, Uint8 g, Uint8 b, Uint8 a) = 0;
	virtual void drawSquare(float x, float y, float width, float height, Uint8 r, Uint8 g, Uint8 b, Uint8 a) = 0;
	virtual void onAfterDraw() = 0;

	virtual Asset* createTexture(const std::string& path, const std::string& assetTag) = 0;
	virtual Asset* createTextAsset(const std::string& text, const std::string& assetTag, Uint8 r, Uint8 g, Uint8 b, Uint8 a) = 0;
protected:
	std::unique_ptr<AssetVendor> mAssetVendor{ nullptr };
};

class SDLGraphics : public Graphics {
public:
	SDLGraphics(GraphicsConfig* graphisConfig, AssetVendor* assetVendor);
	void onBeforeDraw() override;
	void renderTexture(Texture* texture, float x, float y, float w, float h, Uint8 r, Uint8 g, Uint8 b, Uint8 a) override;
	void drawLine(float x0, float y0, float x1, float y1, Uint8 r, Uint8 g, Uint8 b, Uint8 a) override;
	void drawSquare(float x, float y, float width, float height, Uint8 r, Uint8 g, Uint8 b, Uint8 a) override;
	void onAfterDraw() override;

	SDL_Renderer* getRenderer();
	Asset* createTexture(const std::string& path, const std::string& assetTag) override;
	Asset* createTextAsset(const std::string& text, const std::string& assetTag, Uint8 r, Uint8 g, Uint8 b, Uint8 a) override;
private:

	std::unique_ptr<SDL_Window, SDL_DELETERS> mWindow{ nullptr };
	std::unique_ptr<SDL_Renderer, SDL_DELETERS> mRenderer{ nullptr };
	std::unique_ptr<TTF_Font, SDL_DELETERS> mFont{ nullptr };
};

class DrawableComponent : public Component {
public:
	DrawableComponent(unsigned long entityId, Drawable* drawable) : Component(entityId, ComponentType::DRAWABLE_COMPONENT) {
		mDrawable = drawable;
	}

	DrawableComponent(unsigned long entityId, const rapidjson::Value& root) : Component(entityId, ComponentType::DRAWABLE_COMPONENT) {
		const rapidjson::Value& drawable = root["mDrawable"];
		std::string drawableType = drawable["drawableType"].GetString();
		if (drawableType == "BlockDrawable") {
			mDrawable = new BlockDrawable(drawable);
		}
		else if (drawableType == "TextureDrawable") {
			mDrawable = new TextureDrawable(drawable);
		}
		else {
			assert(false);
		}
	}

	void setColor(Uint8 r, Uint8 g, Uint8 b, Uint8 a);
	void setSize(float width, float height);
	Drawable* getDrawable() {
		return mDrawable;
	}

	void serialize(Serializer& serializer) const override {
		serializer.writer.StartObject();

		serializer.writer.String("componentId");
		serializer.writer.Uint(componentId);
		serializer.writer.String("mDrawable");
		mDrawable->serialize(serializer);

		serializer.writer.EndObject();
	}
private:
	Drawable* mDrawable;
};

#endif // !__GRAPHICS_H__
