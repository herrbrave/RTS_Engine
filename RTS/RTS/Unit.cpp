#include"Unit.h"

void Unit::damage(Uint16 damage) {
	mHealth -= damage;
}

bool Unit::isDead() {
	return mHealth <= 0;
}

/* Parent methods */
void Unit::onUpdate() {
}

Unit* UnitFactory::create() {
	Body* unitBody = new BlockBody(0, 0, TILE_WIDTH, TILE_WIDTH);
	Drawable* blockDrawable = new BlockDrawable(TILE_WIDTH, TILE_HEIGHT, 255, 0, 0, 255);

	Unit* unit = new Unit(unitBody, blockDrawable, 100);
	mPhysicsSystem->registerBody(unit->getId(), unitBody);
	mGraphicsSystem->registerDrawable(unit->getId(), blockDrawable);

	return unit;
}