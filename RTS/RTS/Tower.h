#ifndef __TOWER_H__
#define __TOWER_H__

#include<memory>
#include<vector>

#include"ShootState.h"
#include"Unit.h"

class Tower : public Entity {
public:
	Tower(Body* body, Drawable* drawable) : Entity(body, drawable) {
	}

	virtual void onUpdate();
};

#endif // !__TOWER_H__
