#pragma once

#ifndef __WORLD_H__
#define __WORLD_H__

#include"Constants.h"
#include"Entity.h"
#include"Map.h"


class World;
typedef shared_ptr<World> WorldPtr;
typedef weak_ptr<World> WeakWorldPtr;

class World : public Serializable {
public:
	vector<EntityPtr> entities;
	EntityMatrix grids;
	MapPtr map;
	EntityPtr script;

	World() {}

	World(const rapidjson::Value& root);

	EntityPtr getGridAtPoint(const Vector2f& point);

	EntityPtr getGridAt(int x, int y) const;

	MapPtr getMap();

	void serialize(Serializer& serializer) const;
};

#endif