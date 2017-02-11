#ifndef __UI_H__
#define __UI_H__

#include<algorithm>
#include<fstream>
#include<iostream>
#include<memory>
#include<SDL.h>
#include<SDL_ttf.h>
#include<string>
#include<unordered_map>

#include"EntityFactory.h"
#include"Input.h"
#include"SDL_Helpers.h"
#include"Serializer.h"
#include"System.h"
#include"Texture.h"

enum Location {
	MIDDLE,
	TOP,
	PAGE_START
};

enum class ButtonState {
	UP,
	OVER,
	DOWN
};

class ButtonConfig;
typedef shared_ptr<ButtonConfig> ButtonConfigPtr;
typedef weak_ptr<ButtonConfig> WeakButtonConfigPtr;

class PanelConfig;
typedef shared_ptr<PanelConfig> PanelConfigPtr;
typedef weak_ptr<PanelConfig> WeakPanelConfigPtr;

class SectionDrawable;
typedef shared_ptr<SectionDrawable> SectionDrawablePtr;
typedef weak_ptr<SectionDrawable> WeakSectionDrawablePtr;

class ButtonDrawable;
typedef shared_ptr<ButtonDrawable> ButtonDrawablePtr;
typedef weak_ptr<ButtonDrawable> WeakButtonDrawablePtr;

class PanelDrawable;
typedef shared_ptr<PanelDrawable> PanelDrawablePtr;
typedef weak_ptr<PanelDrawable> WeakPanelDrawablePtr;

class ButtonComponent;
typedef shared_ptr<ButtonComponent> ButtonComponentPtr;
typedef weak_ptr<ButtonComponent> WeakButtonComponentPtr;

class WidgetFactory;
typedef shared_ptr<WidgetFactory> WidgetFactoryPtr;
typedef weak_ptr<WidgetFactory> WeakWidgetFactoryPtr;

class ProgressBar {
private:
	Location mLocation;
	int mMaxProgress;
	int mProgress;
public:
	ProgressBar(int maxProgress) : ProgressBar(maxProgress, MIDDLE) {}
	ProgressBar(int maxProgress, Location location) {
		mMaxProgress = mProgress = maxProgress;
		mLocation = location;
	}

	int getProgress() {
		return mProgress;
	}

	int getMaxProgress() {
		return mMaxProgress;
	}

	void setProgress(int progress) {
		mProgress = std::min(std::max(0, progress), mMaxProgress);
	}

	void draw(SDL_Renderer* renderer, SDL_Rect* rect) {
		// padding is two pixels.
		int padding(2);
		int width(rect->w);
		int height(8);
		float percentile((float)mProgress / (float)mMaxProgress);
		int innerwidth((width - 2 * padding) * percentile);
		SDL_Rect outerRect{ 0, 0, 0, 0 };
		SDL_Rect innerRect{ 0, 0, 0, 0 };

		if (mLocation == MIDDLE) {
			outerRect.x = rect->x;
			outerRect.y = rect->y + (rect->h / 2) - (height / 2);
			outerRect.w = width;
			outerRect.h = height;

			innerRect.x = rect->x;
			innerRect.y = rect->y + (rect->h / 2) - (height / 2) + padding;
			innerRect.w = innerwidth;
			innerRect.h = height - 2 * padding;
		}
		else {
			outerRect.x = rect->x - (rect->w / 2);
			outerRect.y = rect->y - (rect->h / 2) - height;
			outerRect.w = width;
			outerRect.h = height;

			innerRect.x = rect->x - (rect->w / 2) + padding;
			innerRect.y = rect->y - (rect->h / 2) - height + padding;
			innerRect.w = innerwidth;
			innerRect.h = height - 2 * padding;
		}

		SDL_SetRenderDrawColor(renderer, 64, 64, 64, 255);
		SDL_RenderFillRect(renderer, &outerRect);

		float redScalar(((float)mMaxProgress - (float)mProgress) / (float)mMaxProgress);
		float greenScalar((float)mProgress / (float)mMaxProgress);

		SDL_SetRenderDrawColor(renderer, 255 * redScalar, 255 * greenScalar, 0, 255);
		SDL_RenderFillRect(renderer, &innerRect);
	}
};

class Label {
private:
	std::unique_ptr<SDL_Texture, SDL_DELETERS> mTexture;
	std::string mText;
	Location mLocation;

public:
	Label(std::string text, TTF_Font* font, SDL_Renderer* renderer, Location location) {
		mText = text;
		mLocation = location;

		SDL_Color textColor = { 255, 255, 255, 255 };
		SDL_Surface* surface = TTF_RenderText_Solid(font, text.c_str(), textColor);
		mTexture = std::unique_ptr<SDL_Texture, SDL_DELETERS>(
			SDL_CreateTextureFromSurface(renderer, surface)
			);
		SDL_FreeSurface(surface);
	}

	void draw(SDL_Renderer* renderer, SDL_Rect* location) {
		SDL_Rect textRect{ 0, 0, 0, 0 };
		SDL_QueryTexture(mTexture.get(), nullptr, nullptr, &textRect.w, &textRect.h);

		SDL_Rect drawLocation;
		switch (mLocation) {
		case TOP:
			drawLocation = {
				location->x - textRect.w / 2,
				location->y - textRect.h - 25,
				textRect.w,
				textRect.h
			};
			break;
		case PAGE_START:
			drawLocation = {
				location->x,
				location->y,
				textRect.w,
				textRect.h
			};
			break;
		case MIDDLE:
			drawLocation = {
				location->x + (location->w / 2) - (textRect.w / 2),
				location->y + (location->h / 2) - (textRect.h / 2),
				textRect.w,
				textRect.h
			};
			break;
		default:
			throw std::runtime_error("Invalid location for lable: " + mLocation);
			break;
		}

		

		SDL_RenderCopy(renderer, mTexture.get(), nullptr, &drawLocation);
	}
};

class ButtonConfig {
public:
	int sectionWidth;
	int sectionHeight;
	unordered_map<std::string, TexturePtr> buttonUp;
	unordered_map<std::string, TexturePtr> buttonOver;
	unordered_map<std::string, TexturePtr> buttonDown;
};

ButtonConfigPtr createButtonConfig(const std::string& path, SystemManagerPtr systemManager);
void parseState(const std::string& tag, const rapidjson::Value& button, std::unordered_map<std::string, TexturePtr>& stateMap);


class SectionDrawable : Drawable {
public:
	SectionDrawable(float width, float height, Vector2f* positionOffset, TexturePtr texture);

	void draw(Graphics& graphicsRef, const Vector2f& position) override;

protected:
	void onSerialize(Serializer& serializer) const override {}

private:
	TexturePtr mTexture;
	Vector2fPtr mPosOffset;
};

class ButtonDrawable : public Drawable {
public:
	ButtonState state;
	ButtonDrawable(float width, float height, const ButtonConfig& buttonConfig);

	void draw(Graphics& graphicsRef, const Vector2f& position) override;

	void setButtonTexture(TexturePtr);

protected:
	void onSerialize(Serializer& serializer) const override {}

private:
	ButtonConfigPtr mButtonConfig{ nullptr };
	unordered_map<ButtonState, std::vector<SectionDrawablePtr>> mSections;
	TexturePtr mButtonTexture{ nullptr };
};

class ButtonComponent : public InputComponent {
public:
	ButtonComponent(unsigned long entityId, DrawablePtr drawable, InputListenerPtr inputListener) 
		: InputComponent(entityId, inputListener, ComponentType::BUTTON_COMPONENT) {

		inputListener->eventCallbacks.emplace(
			Input::ON_MOUSE_ENTER,
			[drawable](EventPtr evt) {
			ButtonDrawable* buttonDrawable = (ButtonDrawable*) drawable.get();
			buttonDrawable->state = ButtonState::OVER;
			return true;
		});
		inputListener->eventCallbacks.emplace(
			Input::ON_MOUSE_EXIT,
			[drawable](EventPtr evt) {
			ButtonDrawable* buttonDrawable = (ButtonDrawable*)drawable.get();
			buttonDrawable->state = ButtonState::UP;
			return true;
		});
		inputListener->eventCallbacks.emplace(
			Input::ON_MOUSE_DOWN,
			[drawable](EventPtr evt) {
			ButtonDrawable* buttonDrawable = (ButtonDrawable*)drawable.get();
			buttonDrawable->state = ButtonState::DOWN;
			return true;
		});
		inputListener->eventCallbacks.emplace(
			Input::ON_CLICK,
			[drawable, this](EventPtr evt) {
			ButtonDrawable* buttonDrawable = (ButtonDrawable*)drawable.get();
			buttonDrawable->state = ButtonState::OVER;
			this->getCallback()();
			return true;
		});
	}

	void onMouseEvent(const MouseEvent& mouseEvent, SystemManagerPtr systemManager);

	void serialize(Serializer& serializer) const override {/* no op */}

	void setText(const std::string& text, SystemManagerPtr systemManager);

	std::string& getText();

	void setIcon(TexturePtr texture, SystemManagerPtr systemManager);

	void setCallback(function<void()>& callback);

private:
	std::string mText;
	function<void()> mCallback;

	function<void()>& getCallback() {
		return mCallback;
	}
};

void setButtonText(EntityPtr entity, const std::string& text, SystemManagerPtr systemManager);
void setIcon(EntityPtr entity, const std::string& assetTag, float tx, float ty, float tw, float th, SystemManagerPtr systemManager);

class PanelConfig {
public:
	int sectionWidth;
	int sectionHeight;
	unordered_map<std::string, TexturePtr> panelSections;
};
PanelConfigPtr createPanelConfig(const std::string& path, SystemManagerPtr systemManager);

class PanelDrawable : public Drawable {
public:
	PanelDrawable(float width, float height, const PanelConfig& panelConfig);

	void draw(Graphics& graphicsRef, const Vector2f& position) override;

protected:
	void onSerialize(Serializer& serializer) const override {}

private:
	PanelConfigPtr mPanelConfig{ nullptr };
	vector<SectionDrawablePtr> mSections;
};

class WidgetFactory : public EntityFactory {
public:
	WidgetFactory(string buttonConfigPath, std::string panelConfigPath, SystemManagerPtr systemManager);

	EntityPtr createButton(std::function<void()> callback, float x, float y, float width, float height);
	EntityPtr createPanel(float x, float y, float width, float height);

private:
	ButtonConfigPtr mButtonConfig{ nullptr };
	PanelConfigPtr mPanelConfig{ nullptr };
};

#endif