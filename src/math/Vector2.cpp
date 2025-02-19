#include "Vector2.h"

namespace nx
{
	//
	// operator==
	//
	bool Vector2::operator==(const Vector2& vec) const
	{
		if (x == vec.x &&
			y == vec.y)
		{
			return true;
		}
		return false;
	}


	//
	// operator!=
	//
	bool Vector2::operator!=(const Vector2& vec) const
	{
		if (x != vec.x ||
			y != vec.y)
		{
			return true;
		}
		return false;
	}


	//
	// operator+
	//
	Vector2 Vector2::operator+(const Vector2& vec) const
	{
		return Vector2(x + vec.x, y + vec.y);
	}


	//
	// operator-
	//
	Vector2 Vector2::operator-(const Vector2& vec) const
	{
		return Vector2(x - vec.x, y - vec.y);
	}


	//
	// operator+=
	//
	void Vector2::operator+=(const Vector2& vec)
	{
		x += vec.x;
		y += vec.y;
	}


	//
	// operator-=
	//
	void Vector2::operator-=(const Vector2& vec)
	{
		x -= vec.x;
		y -= vec.y;
	}


	//
	// operator* scalar
	//
	Vector2 Vector2::operator*(const float& scalar) const
	{
		return Vector2(x * scalar, y * scalar);
	}


	//
	//operator* vector
	//
	Vector2 Vector2::operator*(const Vector2& vec) const
	{
		return Vector2(x * vec.x, y * vec.y);
	}


	//
	// operator/
	//
	Vector2 Vector2::operator/(const float& scalar) const
	{
		return Vector2(x / scalar, y / scalar);
	}


	//
	// operator/ vector
	//
	Vector2 Vector2::operator/(const Vector2& vec) const
	{
		return Vector2(x / vec.x, y / vec.y);
	}


	//
	// operator*= scalar
	//
	void Vector2::operator*=(const float& scalar)
	{
		x *= scalar;
		y *= scalar;
	}


	//
	// operator*= vector
	//
	void Vector2::operator*=(const Vector2& vec)
	{
		x *= vec.x;
		y *= vec.y;
	}


	//
	// operator/= scalar
	//
	void Vector2::operator/=(const float& scalar)
	{
		x /= scalar;
		y /= scalar;
	}


	//
	// operator/= vector
	//
	void Vector2::operator/=(const Vector2& vec)
	{
		x /= vec.x;
		y /= vec.y;
	}


	//
	// dot
	//
	float Vector2::dot(const Vector2& vec) const
	{
		return x*vec.x + y*vec.y;
	}


	//
	// length
	//
	float Vector2::length() const
	{
		float lengthSquared = std::pow(x, 2) + std::pow(y, 2);

		return std::sqrt(lengthSquared);
	}


	//
	// normalize
	//
	void Vector2::normalize()
	{
		float _length = length();

		if (_length == 0)
			return;

		*this /= _length;
	}


	Vector2 Vector2::normalized()
	{
		return *this / length();
	}



	float Vector2::scalar_projection(const Vector2 & b) const
	{
		return this->dot(b) / b.length();
	}



	Vector2 Vector2::vector_projection(Vector2 b) const
	{
		b.normalize();
		return b * this->dot(b);
	}



	Vector2 Vector2::vector_projection(Vector2 b, const float scalar_proj) const
	{
		b.normalize();
		return b * scalar_proj;
	}



	Vector2 Vector2::vector_rejection(const Vector2 & b) const
	{
		return *this - vector_projection(b);
	}



	Vector2 Vector2::vector_rejection(const Vector2 & b, const float scalar_proj) const
	{
		return *this - vector_projection(b, scalar_proj);
	}


	float Vector2::angle(const Vector2 & vec) const
	{
		return atan2(vec.y, vec.x) - atan2(y, x);
	}
}

