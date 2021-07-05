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

// OpenGL requirements
#include<gl\glew.h>
#include<SDL_opengl.h>



class Graphics;
typedef shared_ptr<Graphics> GraphicsPtr;
typedef weak_ptr<Graphics> WeakGraphicsPtr;

class GraphicsConfig;
typedef shared_ptr<GraphicsConfig> GraphicsConfigPtr;
typedef weak_ptr<GraphicsConfig> WeakGraphicsConfigPtr;

class Drawable;
typedef shared_ptr<Drawable> DrawablePtr;
typedef weak_ptr<Drawable> WeakDrawablePtr;

class TextureDrawable;
typedef shared_ptr<TextureDrawable> TextureDrawablePtr;
typedef weak_ptr<TextureDrawable> WeakTextureDrawablePtr;

class TextDrawable;
typedef shared_ptr<TextDrawable> TextDrawablePtr;
typedef weak_ptr<TextDrawable> WeakTextDrawablePtr;


class DrawableComponent;
typedef shared_ptr<DrawableComponent> DrawableComponentPtr; 
typedef weak_ptr<DrawableComponent> WeakDrawableComponentPtr;

enum DrawableType : Uint8 {
	BASE = 0,
	BLOCK = 1,
	TEXTURE = 2,
	TEXT = 3,
	BUTTON = 4,
	PANEL = 5,
	SECTION = 6,
	PROGRESS_BAR = 7,
};

class Drawable {
public:
	float width;
	float height;
	float angle;
	bool isUi{ false };

	Drawable() : Drawable(32, 32) {}
	Drawable(float width, float height) {
		this->width = width;
		this->height = height;
		this->angle = 0;
		mColor.reset(new SDL_Color{ 255, 255, 255, 255 });
		mDrawDepth = 0;
	}

	Drawable(const rapidjson::Value& root) {
		width = root["width"].GetDouble();
		height = root["height"].GetDouble();
		mColor.reset(new SDL_Color{ 
			(Uint8) root["r"].GetUint(), 
			(Uint8) root["g"].GetUint(),
			(Uint8) root["b"].GetUint(),
			(Uint8) root["a"].GetUint()
		});
		angle = root["angle"].GetDouble();
		mDrawDepth = 0;
	}

	void setDrawDepth(Uint8 depth);

	Uint8 getDrawDepth();

	virtual void draw(Graphics& graphicsRef, const Vector2f& position) = 0;

	void setColor(Uint8 r, Uint8 g, Uint8 b, Uint8 a);

	void setAngle(float angleDegrees);

	float getAngle();

	virtual void setSize(float width, float height);

	virtual DrawableType getType() {
		return DrawableType::BASE;
	}

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
	Uint8 mDrawDepth;
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

	void draw(Graphics& graphicsRef, const Vector2f& position) override;

	DrawableType getType() override {
		return DrawableType::BLOCK;
	}
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

class TextureDrawable : public virtual Drawable {
public:
	TextureDrawable() : Drawable(0, 0) {}
	TextureDrawable(TexturePtr texture) : Drawable(texture->w, texture->h) {
		mTexture = std::move(texture);
	}

	TextureDrawable(const rapidjson::Value& root) : Drawable(0, 0) {
		const rapidjson::Value& texture = root["mTexture"];
		mTexture.reset(new Texture(texture));
	}

	void setTexture(TexturePtr texture) {
		mTexture = std::move(texture);
	}

	void draw(Graphics& graphicsRef, const Vector2f& position) override;

	void setSize(float width, float height) override;

	DrawableType getType() override {
		return DrawableType::TEXTURE;
	}

protected:
	virtual void onSerialize(Serializer& serializer) const {
		serializer.writer.String("drawableType");
		serializer.writer.String("TextureDrawable");

		serializer.writer.String("mTexture");
		mTexture->serialize(serializer);
	}

	TexturePtr mTexture{ nullptr };
};

class TextDrawable : public virtual Drawable {
public:
	TextDrawable() : Drawable(0, 0) {}
	TextDrawable(const string& text, const string& font) : Drawable(0, 0) {
		this->text = std::move(text);
		this->font = std::move(font);
	}

	void setText(const string& text) {
		this->text = std::move(text);
	}

	void setFont(const string& font) {
		this->font = std::move(font);
	}

	void draw(Graphics& graphicsRef, const Vector2f& position) override;

	void setSize(float width, float height) override {};

	DrawableType getType() override {
		return DrawableType::TEXT;
	}

protected:

	virtual void onSerialize(Serializer& serializer) const {
		serializer.writer.StartObject();

		serializer.writer.String("drawableType");
		serializer.writer.String("TextDrawable");

		serializer.writer.String("text");
		serializer.writer.String(text.c_str());

		serializer.writer.String("font");
		serializer.writer.String(font.c_str());

		serializer.writer.EndObject();
	}

	string text;
	string font;
};


class GraphicsConfig {
public:
	void setTitle(const string& title);
	void setWindowX(int x);
	void setWindowY(int y);
	void setWindowWidth(int width);
	void setWindowHeight(int height);
	void fullscreen();
	void withOpenGL();
	void borderless();
	void resizable();
	void maximized();
	void setFont(const string& fontPath);
	void setFontSize(int fontSize);

	~GraphicsConfig() {
	}

	std::string mTitle;
	std::string mFontPath;
	int mWindowX = 0;
	int mWindowY = 0;
	int mWidth = 800;
	int mHeight = 600;
	int mFontSize = 12;
	float mZoom = 1.0f;
	Uint32 mFlags{ SDL_WINDOW_SHOWN };
};

class Graphics {
public:
	Graphics(GraphicsConfigPtr graphicsConfig, AssetVendorPtr assetVendor) : mAssetVendor(assetVendor), mGraphicsConfig(graphicsConfig) { }
	virtual void onBeforeDraw() = 0;
	virtual void renderTexture(TexturePtr texture, float x, float y, float w, float h, float angle, Uint8 r, Uint8 g, Uint8 b, Uint8 a) = 0;
	virtual void renderText(const string& text, const string& font, float x, float y, Uint8 r, Uint8 g, Uint8 b, Uint8 a) = 0;
	virtual void drawLine(float x0, float y0, float x1, float y1, Uint8 r, Uint8 g, Uint8 b, Uint8 a) = 0;
	virtual void drawSquare(float x, float y, float width, float height, Uint8 r, Uint8 g, Uint8 b, Uint8 a) = 0;
	virtual void onAfterDraw() = 0;
	
	void zoomBy(float value) {
		this->mGraphicsConfig->mZoom += value;
	}

	virtual AssetPtr createTexture(const std::string& path, const std::string& assetTag) = 0;
	virtual AssetPtr createFontAsset(const std::string& path, const std::string& assetTag, int fontsize) = 0;
protected:
	AssetVendorPtr mAssetVendor{ nullptr };
	GraphicsConfigPtr mGraphicsConfig;
};

class SDLGraphics : public Graphics {
public:
	SDLGraphics(GraphicsConfigPtr graphisConfig, AssetVendorPtr assetVendor);
	void onBeforeDraw() override;
	void renderTexture(TexturePtr texture, float x, float y, float w, float h, float angle, Uint8 r, Uint8 g, Uint8 b, Uint8 a) override;
	void renderText(const string& text, const string& font, float x, float y, Uint8 r, Uint8 g, Uint8 b, Uint8 a) override;
	void drawLine(float x0, float y0, float x1, float y1, Uint8 r, Uint8 g, Uint8 b, Uint8 a) override;
	void drawSquare(float x, float y, float width, float height, Uint8 r, Uint8 g, Uint8 b, Uint8 a) override;
	void onAfterDraw() override;

	AssetPtr createTexture(const std::string& path, const std::string& assetTag) override;
	AssetPtr createFontAsset(const std::string& path, const std::string& assetTag, int fontsize) override;
private:

	std::unique_ptr<SDL_Window, SDL_DELETERS> mWindow{ nullptr };
	std::unique_ptr<SDL_Renderer, SDL_DELETERS> mRenderer{ nullptr };
};

struct GLTexture {
	GLuint texId;
	GLfloat w;
	GLfloat h;
};

struct Vector3f {
	GLfloat x, y, z;
};

class SDLOpenGLGraphics : public Graphics {
public:
	SDLOpenGLGraphics(GraphicsConfigPtr graphisConfig, AssetVendorPtr assetVendor);
	void onBeforeDraw() override;
	void renderTexture(TexturePtr texture, float x, float y, float w, float h, float angle, Uint8 r, Uint8 g, Uint8 b, Uint8 a) override;
	void renderText(const string& text, const string& font, float x, float y, Uint8 r, Uint8 g, Uint8 b, Uint8 a) override;
	void drawLine(float x0, float y0, float x1, float y1, Uint8 r, Uint8 g, Uint8 b, Uint8 a) override;
	void drawSquare(float x, float y, float width, float height, Uint8 r, Uint8 g, Uint8 b, Uint8 a) override;
	void onAfterDraw() override;

	AssetPtr createTexture(const std::string& path, const std::string& assetTag) override;
	AssetPtr createFontAsset(const std::string& path, const std::string& assetTag, int fontsize) override;
private:

	std::unique_ptr<SDL_GLContext, SDL_DELETERS> mGLContext{ nullptr };
	std::unique_ptr<SDL_Window, SDL_DELETERS> mWindow{ nullptr };
};

class DrawableComponent : public Component {
public:
	DrawableComponent(unsigned long entityId, DrawablePtr drawable) : Component(entityId, ComponentType::DRAWABLE_COMPONENT) {
		mDrawable = drawable;
	}

	DrawableComponent(unsigned long entityId, const rapidjson::Value& root) : Component(entityId, ComponentType::DRAWABLE_COMPONENT) {
		const rapidjson::Value& drawable = root["mDrawable"];
		std::string drawableType = drawable["drawableType"].GetString();
		if (drawableType == "BlockDrawable") {
			mDrawable = DrawablePtr(GCC_NEW BlockDrawable(drawable));
		}
		else if (drawableType == "TextureDrawable") {
			mDrawable = DrawablePtr(GCC_NEW TextureDrawable(drawable));
		}
		else {
			assert(false);
		}
	}

	Uint8 getZOrder();
	void setZOrder(Uint8 zOrder);

	void setColor(Uint8 r, Uint8 g, Uint8 b, Uint8 a);
	void setSize(float width, float height);
	void setAngle(float angle);
	float getAngle();
	WeakDrawablePtr getDrawable() {
		return mDrawable;
	}

	void serialize(Serializer& serializer) const override {
		serializer.writer.StartObject();

		serializer.writer.String("componentId");
		serializer.writer.Uint((Uint8) componentId);
		serializer.writer.String("mDrawable");
		mDrawable->serialize(serializer);

		serializer.writer.EndObject();
	}
protected:
	DrawablePtr mDrawable;
};

#endif // !__GRAPHICS_H__
