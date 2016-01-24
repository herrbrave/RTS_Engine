#ifndef __VECTOR_H__
#define __VECTOR_H__

#include<cmath>

class vector2f {
public:
	vector2f(const vector2f& copy) {
		mX = copy.mX;
		mY = copy.mY;
	}

	vector2f(float x, float y) {
		mX = x;
		mY = y;
	}

	void set(vector2f& vector) {
		mX = vector[0];
		mY = vector[1];
	}

	vector2f operator*(float scalar) const {
		return vector2f(mX * scalar, mY * scalar);
	}

	vector2f operator+(const vector2f& other) const {
		return vector2f(mX + other.mX, mY + other.mY);
	}

	vector2f operator-(const vector2f& other) const {
		return vector2f(mX - other.mX, mY - other.mY);
	}

	vector2f& operator*=(float scalar) {
		mX *= scalar;
		mY *= scalar;
		return *this;
	}

	vector2f& operator+=(const vector2f& other) {
		mX += other.mX; 
		mY += other.mY;
		return *this;
	}

	vector2f& operator-=(const vector2f& other) {
		mX -= other.mX;
		mY -= other.mY;
		return *this;
	}

	vector2f dot(const vector2f& other) const {
		return vector2f(mX * other.mX, mY * other.mY);
	}

	float magnitude() {
		return std::sqrt((mX * mX) + (mY * mY));
	}

	void normalize() {
		float mag = magnitude();
		if (mag == 0) {
			return;
		}
		mX /= mag;
		mY /= mag;
	}

	float operator[] (int index) const {
		if (index == 0) {
			return mX;
		}
		else if(index == 1) {
			return mY;
		}
		else {
			// throw some exception
		}

		return 0;
	}
private:
	float mX, mY;
};

typedef std::shared_ptr<vector2f> p_vector2f;

#endif // !__VECTOR_H__
