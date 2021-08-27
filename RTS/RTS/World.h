#pragma once

#ifndef __WORLD_H__
#define __WORLD_H__

#include<queue>

#include"Constants.h"
#include"Entity.h"
#include"Map.h"


class World;
typedef shared_ptr<World> WorldPtr;
typedef weak_ptr<World> WeakWorldPtr;

class Path {
public:
	vector<Vector2f> path;

	Path() {}

	Path(Path& copy) {
		path = vector<Vector2f>(copy.path);
	}

	int getX(int index) {
		return (int) path[index].x;
	}

	int getY(int index) {
		return (int) path[index].y;
	}

	int size() {
		return path.size();
	}
};

class World : public Serializable {
public:
	vector<EntityPtr> entities;
	EntityMatrix grids;
	MapPtr map;
	EntityPtr script;

	World() {}

	World(const rapidjson::Value& root);

	EntityPtr getGridAtPoint(const Vector2f& point);

	EntityPtr getGridWithTileAt(int x, int y);
	
	EntityPtr getGridAt(int x, int y) const;

	CellPtr getCellAt(int x, int y);

	CellPtr getCellAtPoint(const Vector2f& point);

	bool inBounds(int x, int y);

	bool pointInWorldBounds(const Vector2f& point);

	Path* buildPath(int sx, int sy, int ex, int ey);

	MapPtr getMap();

	void serialize(Serializer& serializer) const;
};

double tileDistance(int x0, int y0, int x1, int y1);

#endif