#ifndef __PHYSICS_H__
#define __PHYSICS_H__

struct Extent {
	float x0, y0, x1, y1;
};

class Body {
public:
	virtual bool checkPoint(vector2f& point) = 0;
	virtual bool checkCollision(Body& body) = 0;
	virtual Extent getExtent() = 0;
};

#endif // !__PHYSICS_H__
