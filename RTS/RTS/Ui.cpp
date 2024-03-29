#include"Ui.h"

void ProgressBarDrawable::draw(Graphics& graphicsRef, const Vector2f& position) {

	// padding is two pixels.
	int padding(2);
	float percentile((float)this->currentProgress / (float)this->progressMax);
	int innerwidth((this->width - 2 * padding) * percentile);

	float redScalar(((float)this->progressMax - (float)this->currentProgress) / (float)this->progressMax);
	float greenScalar((float)this->currentProgress / (float)this->progressMax);

	float innerWidth = 0, xOffset = 0, innerX = 0;

	if (this->currentProgress > 0) {
		innerWidth = this->width * percentile - (2 * padding);
		xOffset = (this->width - innerWidth) / 2;
		innerX = position.x - xOffset + padding;
	}

	graphicsRef.drawSquare(position.x, position.y, this->width, this->height, 64, 64, 64, 255);
	graphicsRef.drawSquare(innerX, position.y, innerWidth, this->height - (2 * padding), 255 * redScalar, 255 * greenScalar, 0, 255);

	if (this->displayProgress) {
		graphicsRef.renderText(std::to_string(this->currentProgress) + " / " + std::to_string(this->progressMax), "adventure-pixels", 12, position.x, position.y, 255, 255, 255, 255);
	}
}

void LabelComponent::setText(const std::string& text) {
	this->textDrawable->setText(text);
}

const string& LabelComponent::getText() {
	return this->textDrawable->getText();
}

void LabelComponent::setFont(const std::string& font) {
	this->textDrawable->setFont(font);
}

const string& LabelComponent::getFont() {
	return this->textDrawable->getFont();
}

void LabelComponent::setFontSize(int fontSize) {
	this->textDrawable->setFontSize(fontSize);
}

int LabelComponent::getFontSize() {
	return this->textDrawable->getFontSize();
}

void LabelComponent::setColor(Uint8 r, Uint8 g, Uint8 b) {
	this->textDrawable->setColor(r, g, b, 255);
}

Uint8 LabelComponent::getZOrder() {
	return this->textDrawable->getDrawDepth();
}

void LabelComponent::setZOrder(Uint8 zOrder) {
	this->textDrawable->setDrawDepth(zOrder);

	EntityZOrderSetEventData* eventData = GCC_NEW EntityZOrderSetEventData(entityId, SDL_GetTicks());
	EventManager::getInstance().pushEvent(eventData);
}

SectionDrawable::SectionDrawable(float width, float height, Vector2f* positionOffset, TexturePtr texture) : Drawable(width, height) {
	mPosOffset = Vector2fPtr(positionOffset);
	mTexture = texture;
	this->isUi = true;
}

void SectionDrawable::draw(Graphics& graphicsRef, const Vector2f& position) {
	Vector2f translated = position + mPosOffset;
	graphicsRef.renderTexture(mTexture, translated.x, translated.y, width, height, 0, mColor->r, mColor->g, mColor->b, mColor->a);
}

ButtonDrawable::ButtonDrawable(float width, float height, ButtonConfigPtr buttonConfig) : Drawable(width, height), mButtonConfig(buttonConfig) {
	state = ButtonState::UP;
	mSections[ButtonState::UP] = vector<SectionDrawablePtr>();
	mSections[ButtonState::OVER] = vector<SectionDrawablePtr>();
	mSections[ButtonState::DOWN] = vector<SectionDrawablePtr>();

	float sectionWidth = buttonConfig->sectionWidth;
	float sectionHeight = buttonConfig->sectionHeight;
	float halfWidth = width / 2;
	float halfHeight = height / 2;
	this->iconWidth = width - (2.0f * sectionWidth);
	this->iconHeight = height - (2.0f * sectionHeight);

	int dirs[] = { -1, 0, 1 };
	std::string names[9] = { "upper_left", "upper_middle", "upper_right", "middle_left", "middle", "middle_right", "bottom_left", "bottom_middle", "bottom_right" };
	float size[9][2] = {
		{ sectionWidth, sectionHeight },
		{ width - (2 * sectionWidth), sectionHeight },
		{ sectionWidth, sectionHeight },
		{ sectionWidth, height -(2 * sectionHeight) },
		{ width - (2 * sectionWidth), height - (2 * sectionHeight) },
		{ sectionWidth, height - (2 * sectionHeight) },
		{ sectionWidth, sectionHeight },
		{ width - (2 * sectionWidth), sectionHeight },
		{ sectionWidth, sectionHeight }
	};

	int count = 0;
	for (int yDir : dirs) {
		for (int xDir : dirs) {
			Vector2f* posOffset(GCC_NEW Vector2f((halfWidth - (sectionWidth / 2)) * xDir, (halfHeight - (sectionHeight / 2)) * yDir));
			std::string name = names[count];

			SectionDrawablePtr upDrawable(GCC_NEW SectionDrawable(size[count][0], size[count][1], posOffset, buttonConfig->buttonUp.at(name)));
			SectionDrawablePtr overDrawable(GCC_NEW SectionDrawable(size[count][0], size[count][1], posOffset, buttonConfig->buttonOver.at(name)));
			SectionDrawablePtr downDrawable(GCC_NEW SectionDrawable(size[count][0], size[count][1], posOffset, buttonConfig->buttonDown.at(name)));
			mSections.at(ButtonState::UP).push_back(upDrawable);
			mSections.at(ButtonState::OVER).push_back(overDrawable);
			mSections.at(ButtonState::DOWN).push_back(downDrawable);
			count++;
		}
	}
	this->isUi = true;
}

void ButtonDrawable::draw(Graphics& graphicsRef, const Vector2f& position) {
	for (auto drawable : mSections[state]) {
		drawable->draw(graphicsRef, position);
	}
	if (this->mTexture != nullptr) {
		graphicsRef.renderTexture(this->mTexture, position.x, position.y, this->iconWidth, this->iconHeight, 0.0f, 255, 255, 255, 255);
	}
	if (this->text.size() > 0) {
		TextDrawable::draw(graphicsRef, position);
	}
}

void ButtonComponent::onMouseEvent(const MouseEvent& mouseEvent, SystemManagerPtr systemManager) {
	PhysicsSystemPtr physicsSystem = systemManager->getSystemByType<PhysicsSystem>(SystemType::PHYSICS);
	GraphicsSystemPtr graphicsSystem = systemManager->getSystemByType<GraphicsSystem>(SystemType::GRAPHICS);

	ButtonDrawablePtr buttonDrawable;
	vector<WeakDrawablePtr> drawables;
	graphicsSystem->getDrawableById(entityId, drawables);
	for (WeakDrawablePtr weakDrawablePtr : drawables) {
		DrawablePtr drawable = weakDrawablePtr.lock();
		if (drawable->getType() == DrawableType::BUTTON) {
			buttonDrawable = dynamic_pointer_cast<ButtonDrawable>(weakDrawablePtr.lock());
			break;
		}
	}
	assert(buttonDrawable != nullptr);

	if (mouseEvent.action == MouseAction::MOVE) {
		BodyPtr body = makeShared(physicsSystem->getBody(entityId));
		if (body->checkPoint(*mouseEvent.position)) {
			buttonDrawable->state = ButtonState::OVER;
		}
		else {
			buttonDrawable->state = ButtonState::UP;
		}
	}
	else if (mouseEvent.action == MouseAction::CLICK_DOWN) {
		if (buttonDrawable->state == ButtonState::OVER) {
			buttonDrawable->state = ButtonState::DOWN;
		}
	}
	else {
		if (buttonDrawable->state == ButtonState::DOWN) {
			buttonDrawable->state = ButtonState::OVER;
		}
	}
}

void ButtonComponent::setText(const std::string& text) {
	buttonDrawable->setText(text);
}

void ButtonComponent::setFont(const std::string& font) {
	buttonDrawable->setFont(font);
}

const string& ButtonComponent::getText() {
	return this->buttonDrawable->getText();
}

const string& ButtonComponent::getFont() {
	return this->buttonDrawable->getFont();
}

void ButtonComponent::setFontSize(int fontSize) {
	this->buttonDrawable->setFontSize(fontSize);
}

int ButtonComponent::getFontSize() {
	return this->buttonDrawable->getFontSize();
}

void ButtonComponent::setIcon(TexturePtr texture) {
	buttonDrawable->setTexture(texture);
}

PanelDrawable::PanelDrawable(float width, float height, PanelConfigPtr panelConfig) : Drawable(width, height) {
	isUi = true;
	float sectionWidth = panelConfig->sectionWidth;
	float sectionHeight = panelConfig->sectionHeight;
	float doubleWidth = (sectionWidth * 2);
	float doubleHeight = (sectionHeight * 2);
	float halfWidth = width / 2;
	float halfHeight = height / 2;

	int dirs[] = { -1, 0, 1 };
	std::string names[9] = { "upper_left", "upper_middle", "upper_right", "middle_left", "middle", "middle_right", "bottom_left", "bottom_middle", "bottom_right" };
	float size[9][2] = {
		{ sectionWidth, sectionHeight },
		{ width - doubleWidth, sectionHeight },
		{ sectionWidth, sectionHeight },
		{ sectionWidth, height - doubleHeight },
		{ width - doubleWidth, height - doubleHeight },
		{ sectionWidth, height - doubleHeight },
		{ sectionWidth, sectionHeight },
		{ width - doubleWidth, sectionHeight },
		{ sectionWidth, sectionHeight }
	};

	int count = 0;
	for (int yDir : dirs) {
		for (int xDir : dirs) {
			Vector2f* posOffset(GCC_NEW Vector2f((halfWidth - (sectionWidth / 2)) * xDir, (halfHeight -(sectionHeight / 2)) * yDir));
			std::string name = names[count];

			SectionDrawablePtr sectionDrawable(GCC_NEW SectionDrawable(size[count][0], size[count][1], posOffset, panelConfig->panelSections.at(name)));
			mSections.push_back(sectionDrawable);
			count++;
		}
	}
	this->isUi = true;
}

void PanelDrawable::draw(Graphics& graphicsRef, const Vector2f& position) {
	for (auto drawable : mSections) {
		drawable->draw(graphicsRef, position);
	}
}


ItemPanelDrawable::ItemPanelDrawable(string name, float width, float height) : Drawable(width, height), name(name), drawableWidth(width), drawableHeight(height) {
	this->drawawbleArea = std::make_shared<Texture>(name, 0, 0, width, height);
	this->isUi = true;
}

void ItemPanelDrawable::update(Graphics& graphicsRef) {
	if (!forceRedraw) {
		return;
	}

	graphicsRef.drawToTexture(name);
	for (ItemPtr item : this->items) {
		for (TexturePtr texture : item->texture) {
			graphicsRef.renderTexture(texture, item->position->x, item->position->y, item->width, item->height, 0, 255, 255, 255, 255);
		}
	}
	graphicsRef.drawToScreen();

	forceRedraw = false;
}

void ItemPanelDrawable::addItem(ItemPtr item) {

	items.push_back(item);
	reconfigure();
	forceRedraw = true;
}

int ItemPanelDrawable::handleClick(const Vector2f& position) {
	int index = 0;
	for (auto item : this->items) {
		int minX = item->position->x - (item->width / 2);
		int minY = item->position->y - (item->height / 2);
		int maxX = item->position->x + (item->width / 2);
		int maxY = item->position->y + (item->height / 2);

		if (minX <= position.x && position.x <= maxX && minY <= position.y <= maxY) {
			return index;
		}
		index++;
	}

	return -1;
}

void ItemPanelDrawable::reconfigure() {
	float itemSize = drawableWidth / columns - ((columns + 1) * margin);

	for (int index = 0; index < items.size(); index++) {
		ItemPtr item = this->items.at(index);

		int row = index / columns;
		item->width = itemSize;
		item->height = itemHeight == 1.0f ? itemSize : itemHeight;

		float column = index % columns;
		float x = column * item->width + ((column + 1) * margin) + (item->width / 2.0f);
		float y = row * item->height + ((row + 1) * margin) + (item->height / 2.0f);
		if (item->position == nullptr) {
			item->position.reset(GCC_NEW Vector2f(x, y));
		}
		else {
			item->position->set(x, y);
		}
	}

	forceRedraw = true;
}

void ItemPanelDrawable::setItemHeight(float itemHeight) {
	this->itemHeight = itemHeight;
	this->reconfigure();
}

void ItemPanelDrawable::draw(Graphics& graphicsRef, const Vector2f& position) {
	this->update(graphicsRef);

	graphicsRef.renderTexture(this->drawawbleArea, position.x, position.y, drawableWidth, drawableHeight, 0.0f, 255, 255, 255, 255);
}

ItemPanelComponent::ItemPanelComponent(unsigned long entityId, PanelDrawablePtr panelDrawable, shared_ptr<ItemPanelDrawable> itemPanelDrawable, InputListenerPtr inputListener) : InputComponent(entityId, inputListener, ComponentType::ITEM_PANEL_COMPONENT), panelDrawable(panelDrawable), itemPanelDrawable(itemPanelDrawable) {
	inputListener->eventCallbacks.emplace(
		Input::ON_CLICK,
		[&](EventPtr evt) {
			selected = itemPanelDrawable->handleClick(evt->mouseEvent->positionRelative);

			return true;
		});
}

void ItemPanelComponent::setColumns(unsigned int columns) {
	this->itemPanelDrawable->columns = columns;
	this->itemPanelDrawable->reconfigure();
}

void ItemPanelComponent::setItemHeight(float itemHeight) {
	this->itemPanelDrawable->setItemHeight(itemHeight);
}

void ItemPanelComponent::pushTexture(TexturePtr texture, int x, int y) {
	this->itemPanelDrawable->items.at(this->getIndex(x, y))->texture.push_back(texture);
	this->itemPanelDrawable->reconfigure();
}

void ItemPanelComponent::popTexture(int x, int y) {
	this->itemPanelDrawable->items.at(this->getIndex(x, y))->texture.pop_back();
	this->itemPanelDrawable->reconfigure();
}

void ItemPanelComponent::addItem(ItemPtr item) {
	this->itemPanelDrawable->addItem(item);
}

void ItemPanelComponent::popItem() {
	this->itemPanelDrawable->items.pop_back();
	this->itemPanelDrawable->reconfigure();
}

void ItemPanelComponent::removeItem(int x, int y) {
	this->itemPanelDrawable->items.erase(this->itemPanelDrawable->items.begin() + this->getIndex(x, y));
	this->itemPanelDrawable->reconfigure();
}

int ItemPanelComponent::getIndex(int x, int y) {
	return (y / this->itemPanelDrawable->columns) + x;
}