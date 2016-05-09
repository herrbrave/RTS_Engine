#ifndef __VECTOR_H__
#define __VECTOR_H__

#include"Serializer.h"

#include<cmath>
#include<memory>

class Vector2f;
typedef shared_ptr<Vector2f> Vector2fPtr;
typedef weak_ptr<Vector2f> WeakVector2fPtr;

class Vector2f {
public:
	Vector2f() : x(0), y(0) {}
	Vector2f(Vector2fPtr copy) {
		x = copy->x;
		y = copy->y;
	}

	Vector2f(float x, float y) {
		this->x = x;
		this->y = y;
	}

	Vector2f(const rapidjson::Value& root) {
		x = root["x"].GetDouble();
		y = root["y"].GetDouble();
	}

	void set(Vector2f* vector) {
		x = vector->x;
		y = vector->y;
	}

	void set(float x, float y) {
		this->x = x;
		this->y = y;
	}

	Vector2f operator*(float scalar) const {
		return Vector2f(x * scalar, y * scalar);
	}

	Vector2f operator+(const Vector2f& other) const {
		return Vector2f(x + other.x, y + other.y);
	}

	Vector2f operator-(const Vector2f& other) const {
		return Vector2f(x - other.x, y - other.y);
	}

	Vector2f& operator*=(float scalar) {
		x *= scalar;
		y *= scalar;
		return *this;
	}

	Vector2f& operator+=(const Vector2f& other) {
		x += other.x; 
		y += other.y;
		return *this;
	}

	Vector2f& operator-=(const Vector2f& other) {
		x -= other.x;
		y -= other.y;
		return *this;
	}

	Vector2f dot(const Vector2f& other) const {
		return Vector2f(x * other.x, y * other.y);
	}

	float magnitude() {
		return std::sqrt((x * x) + (y * y));
	}

	void normalize() {
		float mag = magnitude();
		if (mag == 0) {
			return;
		}
		x /= mag;
		y /= mag;
	}

	float x, y;

	void serialize(Serializer& serializer) const {
		serializer.writer.StartObject();
		serializer.writer.String("x");
		serializer.writer.Double(x);
		serializer.writer.String("y");
		serializer.writer.Double(y);
		serializer.writer.EndObject();
	}
};

#endif // !__VECTOR_H__
