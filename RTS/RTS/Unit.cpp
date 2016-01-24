#include"Unit.h"


void Unit::setCurrentTile(std::shared_ptr<Tile> currentTile) {
	mCurrentTile = currentTile;
}

std::shared_ptr<Tile> Unit::getCurrentTile() {
	return mCurrentTile;
}

void Unit::damage(int damage) {
	mHealthBar->setProgress(mHealthBar->getProgress() - damage);
}

bool Unit::isDead() {
	return mHealthBar->getProgress() == 0;
}

/* Parent methods */
void Unit::update() {
	Entity::update();
}

void Unit::draw(std::shared_ptr<SDL_Renderer> renderer) {
	Entity::draw(renderer);

	SDL_Rect drawRect = mBlock->getRect(mCamera);
	mName->draw(renderer.get() , &drawRect);
	mHealthBar->draw(renderer.get(), &drawRect);
}