#ifndef __VECTOR_H__
#define __VECTOR_H__

#include<cmath>
#include<memory>

class vector2f {
public:
	vector2f(const vector2f& copy) {
		x = copy.x;
		y = copy.y;
	}

	vector2f(float x, float y) {
		this->x = x;
		this->y = y;
	}

	void set(vector2f* vector) {
		x = vector->x;
		y = vector->y;
	}

	void set(float x, float y) {
		x = x;
		y = y;
	}

	vector2f operator*(float scalar) const {
		return vector2f(x * scalar, y * scalar);
	}

	vector2f operator+(const vector2f& other) const {
		return vector2f(x + other.x, y + other.y);
	}

	vector2f operator-(const vector2f& other) const {
		return vector2f(x - other.x, y - other.y);
	}

	vector2f& operator*=(float scalar) {
		x *= scalar;
		y *= scalar;
		return *this;
	}

	vector2f& operator+=(const vector2f& other) {
		x += other.x; 
		y += other.y;
		return *this;
	}

	vector2f& operator-=(const vector2f& other) {
		x -= other.x;
		y -= other.y;
		return *this;
	}

	vector2f dot(const vector2f& other) const {
		return vector2f(x * other.x, y * other.y);
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
};

typedef std::shared_ptr<vector2f> p_vector2f;

#endif // !__VECTOR_H__
