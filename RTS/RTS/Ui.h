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

class ProgressComponent;
typedef shared_ptr<ProgressComponent> ProgressComponentPtr;
typedef weak_ptr<ProgressComponent> WeakProgressComponentPtr;

class Item;
typedef shared_ptr<Item> ItemPtr;
typedef weak_ptr<Item> WeakItemPtr;

class ItemPanelDrawable;
typedef shared_ptr<ItemPanelDrawable> ItemPanelDrawablePtr;
typedef weak_ptr<ItemPanelDrawable> WeakItemPanelDrawablePtr;

class ItemPanelComponent;
typedef shared_ptr<ItemPanelComponent> ItemPanelComponentPtr;
typedef weak_ptr<ItemPanelComponent> WeakItemPanelComponentPtr;

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

	DrawableType getType() override {
		return DrawableType::PROGRESS_BAR;
	}

protected:
	void onSerialize(Serializer& serializer) const override {}
};

class LabelComponent : public Component {
public:
	LabelComponent(unsigned long entityId, TextDrawablePtr textDrawable) : Component(entityId, ComponentType::LABEL_COMPONENT), textDrawable(textDrawable) {
	}

	void serialize(Serializer& serializer) const override {/* no op */ }

	void setText(const std::string& text);

	void setFont(const std::string& font);

private:
	TextDrawablePtr textDrawable;
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

	DrawableType getType() override {
		return DrawableType::SECTION;
	}

protected:
	void onSerialize(Serializer& serializer) const override {}

private:
	TexturePtr mTexture;
	Vector2fPtr mPosOffset;
};

class ButtonDrawable : public TextDrawable, public TextureDrawable {
public:
	ButtonState state;
	ButtonDrawable(float width, float height, ButtonConfigPtr buttonConfig);

	void draw(Graphics& graphicsRef, const Vector2f& position) override;

	DrawableType getType() override {
		return DrawableType::BUTTON;
	}

	void setSize(float w, float h) override {
		Drawable::setSize(w, h);
	}

protected:
	void onSerialize(Serializer& serializer) const override {}

private:
	float iconWidth;
	float iconHeight;
	ButtonConfigPtr mButtonConfig{ nullptr };
	unordered_map<ButtonState, std::vector<SectionDrawablePtr>> mSections;
};

class ButtonComponent : public InputComponent {
public:

	ButtonComponent(unsigned long entityId, ButtonDrawablePtr drawable, InputListenerPtr inputListener, LuaScriptPtr script)
		: InputComponent(entityId, inputListener, ComponentType::BUTTON_COMPONENT), buttonDrawable(drawable) {
		this->initialize(inputListener, drawable, script);
	}

	ButtonComponent(unsigned long entityId, ButtonDrawablePtr drawable, InputListenerPtr inputListener, TextureDrawablePtr icon, LuaScriptPtr script) : InputComponent(entityId, inputListener, ComponentType::BUTTON_COMPONENT), buttonDrawable(drawable) {
		this->icon = icon;
		initialize(inputListener, drawable, script);
	}

	void onMouseEvent(const MouseEvent& mouseEvent, SystemManagerPtr systemManager);

	void serialize(Serializer& serializer) const override {/* no op */}

	void setText(const std::string& text);

	void setFont(const std::string& font);

	void setIcon(TexturePtr texture);

private:
	LuaScriptPtr script;
	ButtonDrawablePtr buttonDrawable;
	TextureDrawablePtr icon;

	void initialize(InputListenerPtr inputListener, ButtonDrawablePtr drawable, LuaScriptPtr luaScript) {
		inputListener->eventCallbacks.emplace(
			Input::ON_MOUSE_ENTER,
			[drawable, luaScript](EventPtr evt) {
				drawable->state = ButtonState::OVER;

				luaScript->invoke("onMouseEnterEntity");

				return true;
			});
		inputListener->eventCallbacks.emplace(
			Input::ON_MOUSE_EXIT,
			[drawable, luaScript](EventPtr evt) {
				drawable->state = ButtonState::UP;

				luaScript->invoke("onMouseExitEntity");

				return true;
			});
		inputListener->eventCallbacks.emplace(
			Input::ON_MOUSE_DOWN,
			[drawable, luaScript](EventPtr evt) {
				drawable->state = ButtonState::DOWN;

				luaScript->invoke("onMouseDown", evt->mouseEvent->position->x, evt->mouseEvent->position->y, evt->mouseEvent->button);

				return true;
			});
		inputListener->eventCallbacks.emplace(
			Input::ON_CLICK,
			[drawable, luaScript](EventPtr evt) {
				drawable->state = ButtonState::OVER;

				luaScript->invoke("onClickEntity", evt->mouseEvent->button);
				luaScript->invoke("onMouseUp", evt->mouseEvent->position->x, evt->mouseEvent->position->y, evt->mouseEvent->button);

				return true;
			});
		this->script = script;
	}
};

class ProgressComponent : public Component {
public:
	ProgressComponent(unsigned long id, WeakProgressBarDrawablePtr progressBarDrawable) : Component(id, ComponentType::PROGRESS_COMPONENT) {
		progressBar = progressBarDrawable;
	}

	void serialize(Serializer& serializer) const override {/* no op */ }

	void setProgress(int progress, int maxProgress) {
		ProgressBarDrawablePtr prog = makeShared(progressBar);
		prog->setProgress(progress);
		prog->setProgressMax(progress);
	}

private:
	WeakProgressBarDrawablePtr progressBar;
};

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

	DrawableType getType() override {
		return DrawableType::PANEL;
	}

protected:
	void onSerialize(Serializer& serializer) const override {}

private:
	PanelConfigPtr mPanelConfig{ nullptr };
	vector<SectionDrawablePtr> mSections;
};

class Item {
public:
	vector<TexturePtr> texture;
	Vector2fPtr position;
	float width;
	float height;
	string name;
	int x;
	int y;
};

class ItemPanelDrawable : public Drawable {
public:
	TexturePtr drawawbleArea;
	vector<ItemPtr> items;
	unsigned int columns = 5;
	float margin = 4.0f;
	string name;

	ItemPanelDrawable(string name, float width, float height);
	
	void update(Graphics& graphicsRef);

	void addItem(ItemPtr item);

	void draw(Graphics& graphicsRef, const Vector2f& position) override;

	DrawableType getType() override {
		return DrawableType::PANEL;
	}

	void reconfigure();

protected:
	void onSerialize(Serializer& serializer) const override {}

private:
	float drawableWidth;
	float drawableHeight;
	bool forceRedraw = true;
};

class ItemPanelComponent : public Component {
public:
	ItemPanelDrawablePtr panelDrawable;

	ItemPanelComponent(unsigned long entityId, ItemPanelDrawablePtr panelDrawable);

	void setColumns(unsigned int columns);

	void pushTexture(TexturePtr texture, int x, int y);

	void popTexture(int x, int y);

	void addItem(ItemPtr item);

	void popItem();

	void removeItem(int x, int y);

	void serialize(Serializer& serializer) const override {/* no op */ }

private: 
	int getIndex(int x, int y);
};

void applyButtonWithText(SystemManagerPtr systemManager, unsigned long entityId, float w, float h, const string& text, const string& font, const string& script, ButtonConfigPtr buttonConfig);
void applyButtonWithIcon(SystemManagerPtr systemManager, unsigned long entityId, float w, float h, TexturePtr icon, const string& script, ButtonConfigPtr buttonConfig);
void applyLabel(SystemManagerPtr systemManager, unsigned long entityId, const string& text, const string& font, Uint8 r, Uint8 g, Uint8 b, Uint8 a);
void applyProgress(SystemManagerPtr systemManager, unsigned long entityId, float w, float h, unsigned int maxProgress, unsigned int currentProgress, const string& location);

class WidgetFactory : public EntityFactory {
public:
	WidgetFactory(string buttonConfigPath, std::string panelConfigPath, SystemManagerPtr systemManager);

	EntityPtr createLabel(std::string text, std::string font, float x, float y);
	EntityPtr createButton(std::function<void()> callback, float x, float y, float width, float height);
	EntityPtr createButtonWithText(const string& text, const string& font, const string& script, float x, float y, float width, float height);
	EntityPtr createButtonWithIcon(TexturePtr icon, const string& script, float x, float y, float width, float height);
	EntityPtr createPanel(float x, float y, float width, float height);
	EntityPtr createProgressBar(float x, float y, float width, float height, unsigned int progressMax, unsigned int currentProgress);

private:
	ButtonConfigPtr mButtonConfig{ nullptr };
	PanelConfigPtr mPanelConfig{ nullptr };
};

#endif