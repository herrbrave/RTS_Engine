#ifndef __VECTOR_H__
#define __VECTOR_H__

#include"Serializer.h"

#include<cmath>
#include<memory>

#define CMP_EPSILON 0.00001

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

	void set(const Vector2f& vector) {
		x = vector.x;
		y = vector.y;
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

	Vector2f operator*(const Vector2f& other) const {
		return Vector2f(x * other.x, y * other.y);
	}

	Vector2f& operator*=(const Vector2f& other) {
		this->x *= this->x * other.x;
		this->y *= this->y * other.y;
		return *this;
	}

	float dot(const Vector2f& other) {
		return this->x * other.x + this->y * other.y;
	}

	Vector2f slide(const Vector2f& normal) {
		return *this - normal * this->dot(normal);
	}

	Vector2f reflect(const Vector2f& normal) {
		return normal * 2.0f * this->dot(normal) - *this;
	}

	Vector2f bounce(const Vector2f& normal) {
		return this->reflect(normal) * -1.0f;
	}

	float magnitude() {
		return std::sqrt((x * x) + (y * y));
	}

	Vector2f moveToward(const Vector2f& to, const float delta) const {
		Vector2f vec(*this);
		Vector2f vectorDelta = to - vec;
		float mag = vectorDelta.magnitude();
		return mag <= delta || mag < CMP_EPSILON ? to : vec + vectorDelta * (delta / mag);
	}

	void normalize() {
		float mag = magnitude();
		if (mag == 0) {
			return;
		}
		x /= mag;
		y /= mag;
	}

	void truncate(float max) {
		if (this->magnitude() < max) {
			return;
		}

		this->normalize();
		this->x *= max;
		this->y *= max;
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

static const Vector2fPtr ZERO_VECTOR{ GCC_NEW Vector2f(0, 0) };

#endif // !__VECTOR_H__
