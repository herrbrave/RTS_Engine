#ifndef __UNIT_H__
#define __UNIT_H__

#include"Entity.h"

class Unit : public Entity {
public:
	Unit(Body* body, Drawable* drawable, Uint16 maxHealth) : Entity(body, drawable){
		mMaxHealth = mHealth = maxHealth;
	}

	void damage(Uint16 damage);
	bool isDead(); 
	
	/* Parent methods */
	void onUpdate() override;

private:
	Uint16 mMaxHealth;
	Uint16 mHealth;
};

class UnitFactory : public EntityFactory {
public:
	UnitFactory(GraphicsSystem* graphics, PhysicsSystem* physics) : EntityFactory(graphics, physics) {}

	Unit* create();
};

#endif // !__UNIT_H__
