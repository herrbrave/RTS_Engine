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

class ProgressBarDrawable;
typedef shared_ptr<ProgressBarDrawable> ProgressBarDrawablePtr;
typedef weak_ptr<ProgressBarDrawable> WeakProgressBarDrawablePtr;

class SectionDrawable;
typedef shared_ptr<SectionDrawable> SectionDrawablePtr;
typedef weak_ptr<SectionDrawable> WeakSectionDrawablePtr;

class ButtonDrawable;
typedef shared_ptr<ButtonDrawable> ButtonDrawablePtr;
typedef weak_ptr<ButtonDrawable> WeakButtonDrawablePtr;

class PanelDrawable;
typedef shared_ptr<PanelDrawable> PanelDrawablePtr;
typedef weak_ptr<PanelDrawable> WeakPanelDrawablePtr;

class LabelComponent;
typedef shared_ptr<LabelComponent> LabelComponentPtr;
typedef weak_ptr<LabelComponent> WeakLabelComponentPtr;

class ButtonComponent;
typedef shared_ptr<ButtonComponent> ButtonComponentPtr;
typedef weak_ptr<ButtonComponent> WeakButtonComponentPtr;

class WidgetFactory;
typedef shared_ptr<WidgetFactory> WidgetFactoryPtr;
typedef weak_ptr<WidgetFactory> WeakWidgetFactoryPtr;



class ProgressBarDrawable : public Drawable {
public:
	unsigned int progressMax;
	unsigned int currentProgress;
	ProgressBarDrawable(float width, float height, unsigned int progressMax, unsigned int currentProgress) : Drawable(width, height) {
		this->progressMax = progressMax;
		this->currentProgress = currentProgress;
	}

	void draw(Graphics& graphicsRef, const Vector2f& position) override;

	void setProgress(unsigned int progress) {
		this->currentProgress = std::min(progress, this->progressMax);
	}

	unsigned int getProgress() {
		return this->currentProgress;
	}

	void setProgressMax(unsigned int progressMax) {
		this->progressMax = progressMax;
	}

	unsigned int getProgressMax() {
		return this->progressMax;
	}

protected:
	void onSerialize(Serializer& serializer) const override {}
};

class LabelComponent : public Component {
public:
	LabelComponent(unsigned long entityId, DrawablePtr drawable)
		: Component(entityId, ComponentType::LABEL_COMPONENT) {
	}

	void serialize(Serializer& serializer) const override {/* no op */ }

	void setText(const std::string& text, SystemManagerPtr systemManager);

	std::string& getText();

private:
	std::string mText;
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

	EntityPtr createLabel(std::string text, float x, float y);
	EntityPtr createButton(std::function<void()> callback, float x, float y, float width, float height);
	EntityPtr createButtonWithText(std::string text, std::function<void()> callback, float x, float y, float width, float height);
	EntityPtr createPanel(float x, float y, float width, float height);
	EntityPtr createProgressBar(float x, float y, float width, float height, unsigned int progressMax, unsigned int currentProgress);

private:
	ButtonConfigPtr mButtonConfig{ nullptr };
	PanelConfigPtr mPanelConfig{ nullptr };
};

#endif