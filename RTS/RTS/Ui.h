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

class TextboxConfig;
typedef shared_ptr<TextboxConfig> TextboxConfigPtr;
typedef weak_ptr<TextboxConfig> WeakTextboxConfigPtr;

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

class ItemPanelComponent;
typedef shared_ptr<ItemPanelComponent> ItemPanelComponentPtr;
typedef weak_ptr<ItemPanelComponent> WeakItemPanelComponentPtr;

class TextboxConfig;
typedef shared_ptr<TextboxConfig> TextboxConfigPtr;
typedef weak_ptr<TextboxConfig> WeakTextboxConfigPtr;

class TextboxDrawable;
typedef shared_ptr<TextboxDrawable> TextboxDrawablePtr;
typedef weak_ptr<TextboxDrawable> WeakTextboxDrawablePtr;

class TextboxComponent;
typedef shared_ptr<TextboxComponent> TextboxComponentPtr;
typedef weak_ptr<TextboxComponent> WeakTextboxComponentPtr;

class ButtonConfig {
public:
	int sectionWidth;
	int sectionHeight;
	unordered_map<std::string, TexturePtr> buttonUp;
	unordered_map<std::string, TexturePtr> buttonOver;
	unordered_map<std::string, TexturePtr> buttonDown;
};

class PanelConfig {
public:
	int sectionWidth;
	int sectionHeight;
	unordered_map<std::string, TexturePtr> panelSections;
};

class TextboxConfig {
public:
	unordered_map<std::string, TexturePtr> textboxSections;
	float sectionWidth;
};

class UIConfig {
public:
	ButtonConfigPtr buttonConfig;
	PanelConfigPtr panelConfig;
	TextboxConfigPtr textboxConfig;
	string font;
	string fontTag;
};

class ProgressBarDrawable : public Drawable {
public:
	bool displayProgress = true;
	unsigned int progressMax;
	unsigned int currentProgress;
	ProgressBarDrawable(float width, float height, unsigned int progressMax, unsigned int currentProgress) : Drawable(width, height) {
		this->progressMax = progressMax;
		this->currentProgress = currentProgress;
		this->isUi = true;
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
		textDrawable->isUi = true;
	}

	void serialize(Serializer& serializer) const override {/* no op */ }

	void setText(const string& text);

	const string& getText();

	void setFont(const string& font);

	const string& getFont();

	void setFontSize(int fontSize);

	int getFontSize();

	void setColor(Uint8 r, Uint8 g, Uint8 b);

	Uint8 getZOrder();

	void setZOrder(Uint8 zOrder);

private:
	TextDrawablePtr textDrawable;
};

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

	void setText(const string& text);

	const string& getText();

	void setFont(const string& font);

	const string& getFont();

	void setFontSize(int fontSize);

	int getFontSize();

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
	ProgressComponent(unsigned long id, ProgressBarDrawablePtr progressBarDrawable) : Component(id, ComponentType::PROGRESS_COMPONENT) {
		progressBar = progressBarDrawable;
	}

	void serialize(Serializer& serializer) const override {/* no op */ }

	void setProgress(int progress, int maxProgress) {
		ProgressBarDrawablePtr prog = progressBar;
		prog->setProgress(progress);
		prog->setProgressMax(maxProgress);
	}

	void displayProgress() {
		this->progressBar->displayProgress = true;
	}

	void hideProgress() {
		this->progressBar->displayProgress = false;
	}

	ProgressBarDrawablePtr progressBar;
};

class PanelDrawable : public Drawable {
public:
	PanelDrawable(float width, float height, PanelConfigPtr panelConfig);

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

	void setItemHeight(float itemHeight);

	void draw(Graphics& graphicsRef, const Vector2f& position) override;

	int handleClick(const Vector2f& position);

	DrawableType getType() override {
		return DrawableType::PANEL;
	}

	void reconfigure();

protected:
	void onSerialize(Serializer& serializer) const override {}

private:
	float drawableWidth;
	float drawableHeight;
	float itemHeight = -1.0f;
	bool forceRedraw = true;
};

typedef shared_ptr<ItemPanelDrawable> ItemPanelDrawablePtr;
typedef weak_ptr<ItemPanelDrawable> WeakItemPanelDrawablePtr;

class ItemPanelComponent : public InputComponent {
public:
	PanelDrawablePtr panelDrawable;
	shared_ptr<ItemPanelDrawable> itemPanelDrawable;

	ItemPanelComponent(unsigned long entityId, PanelDrawablePtr panelDrawable, shared_ptr<ItemPanelDrawable> itemPanelDrawable, InputListenerPtr inputListener);

	void setColumns(unsigned int columns);

	void setItemHeight(float itemHeight);

	void pushTexture(TexturePtr texture, int x, int y);

	void popTexture(int x, int y);

	void addItem(ItemPtr item);

	void popItem();

	void removeItem(int x, int y);

	void serialize(Serializer& serializer) const override {/* no op */ }

private:
	int getIndex(int x, int y);

	int selected = -1;
};

class TextboxDrawable : public TextDrawable {
public:
	TextboxDrawable(float width, float height, const string& text, const string& font, int fontSize, TextboxConfigPtr textboxConfig) : TextDrawable(text, font, fontSize) {
		Drawable::width = width;
		Drawable::height = height;
		this->left = textboxConfig->textboxSections["left"];
		this->middle = textboxConfig->textboxSections["middle"];
		this->right = textboxConfig->textboxSections["right"];
		this->sectionWidth = textboxConfig->sectionWidth;
		this->cursorIndex = text.length();
		this->isUi = true;
	}

	void setText(const string& text) {
		this->text = text;
		this->cursorIndex = text.length();
	}

	const string& getText() {
		return this->text;
	}

	void addText(const string& text) {
		this->text.insert(this->cursorIndex, text);
		this->cursorIndex += text.length();
	}

	void backspace() {
		if (this->text.empty()) {
			return;
		}

		this->text.erase(this->text.begin() + (this->cursorIndex - 1));
		this->decCursor();
	}

	void deleteText() {
		if (this->text.empty() || this->cursorIndex >= this->text.length()) {
			return;
		}

		this->text.erase(this->text.begin() + (this->cursorIndex));
	}

	void decCursor() {
		this->cursorIndex = std::max(0, this->cursorIndex - 1);
	}

	void incCursor() {
		this->cursorIndex = std::min((int) this->text.length(), this->cursorIndex + 1);
	}

	void maxCursor() {
		this->cursorIndex = this->text.length();
	}

	void minCursor() {
		this->cursorIndex = 0;
	}

	void draw(Graphics& graphicsRef, const Vector2f& position) override {
		int diff = (this->width / 2) - (this->sectionWidth / 2);
		int textWidth = graphicsRef.getTextWidth(this->text, this->font, fontSize);
		int textHeight = graphicsRef.getTextHeight(this->text, this->font, fontSize);

		graphicsRef.renderTexture(left, position.x - diff, position.y, this->sectionWidth, this->height, 0.0f, 255, 255, 255, 255);
		graphicsRef.renderTexture(middle, position.x, position.y, this->width - (2.0f * this->sectionWidth), this->height, 0.0f, 255, 255, 255, 255);
		graphicsRef.renderTexture(right, position.x + diff, position.y, this->sectionWidth, this->height, 0.0f, 255, 255, 255, 255);

		int textStart = (this->width / 2) - (this->sectionWidth + (textWidth / 2));
		graphicsRef.renderText(text, font, fontSize, position.x - textStart, position.y, 255, 255, 255, 255);

		int cursor = graphicsRef.getTextWidth(this->text.substr(0, this->cursorIndex), font, fontSize);
		int cursorPos = (this->width / 2) - (this->sectionWidth + cursor);
		graphicsRef.drawSquare(position.x - cursorPos, position.y, 3, textHeight, 255, 255, 255, 128);
	}

	DrawableType getType() override {
		return DrawableType::TEXT;
	}

protected:
	void onSerialize(Serializer& serializer) const override {}

private:
	TexturePtr left;
	TexturePtr middle;
	TexturePtr right;
	float sectionWidth;
	int cursorIndex = 0;
};

class TextboxComponent : public InputComponent {
public:
	TextboxDrawablePtr textbox;

	TextboxComponent(unsigned long entityId, TextboxDrawablePtr textbox, InputListenerPtr inputListener)
		: InputComponent(entityId, inputListener, ComponentType::TEXT_BOX_COMPONENT), textbox(textbox) {

		inputListener->eventCallbacks.emplace(
			Input::ON_TEXT_INPUT,
			[textbox](EventPtr evt) {

				textbox->addText(evt->keyEvent->text);

				return true;
			});
		inputListener->eventCallbacks.emplace(
			Input::ON_KEY_DOWN,
			[textbox](EventPtr evt) {

				if (evt->keyEvent->key == SDLK_LEFT) {
					textbox->decCursor();
				}
				else if (evt->keyEvent->key == SDLK_RIGHT) {
					textbox->incCursor();
				}
				else if (evt->keyEvent->key == SDLK_END) {
					textbox->maxCursor();
				}
				else if (evt->keyEvent->key == SDLK_HOME) {
					textbox->minCursor();
				}
				else if (evt->keyEvent->key == SDLK_BACKSPACE) {
					textbox->backspace();
				}
				else if (evt->keyEvent->key == SDLK_DELETE) {
					textbox->deleteText();
				}

				return true;
			});
		inputListener->eventCallbacks.emplace(
			Input::ON_MOUSE_ENTER,
			[textbox](EventPtr evt) {

				return true;
			});
		inputListener->eventCallbacks.emplace(
			Input::ON_MOUSE_EXIT,
			[textbox](EventPtr evt) {

				return true;
			});
		inputListener->eventCallbacks.emplace(
			Input::ON_MOUSE_DOWN,
			[textbox](EventPtr evt) {

				return true;
			});
		inputListener->eventCallbacks.emplace(
			Input::ON_CLICK,
			[textbox](EventPtr evt) {

				return true;
			});
	}

	void setText(const string& text) {
		this->textbox->setText(text);
	}

	void addText(const string& text) {
		this->textbox->addText(text);
	}

	const string& getText() {
		return this->textbox->getText();
	}

	void setFont(const string& font) {
		this->textbox->setFont(font);
	}


	void setFontSize(int fontSize) {
		this->textbox->setFontSize(fontSize);
	}

	int getFontSize() {
		return this->textbox->getFontSize();
	}
};

#endif