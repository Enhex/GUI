#pragma once

//#include <cereal\archives\json.hpp>
#include <cmath>

namespace nx
{
	//
	// A self contained definition of a 2D vector. Used for flexibility.
	//
	struct Vector2
	{
		Vector2() = default;
		constexpr Vector2(float new_x, float new_y) noexcept :
			x(new_x),
			y(new_y){}

		union{
			struct{ float x, y; };
			float component[2];
		};

		constexpr float operator[](size_t pos) const{
			return component[pos];
		}
		constexpr float& operator[](size_t pos){
			return component[pos];
		}

		// Vector equal
		bool operator==(const Vector2&) const;

		// Vector not equal
		bool operator!=(const Vector2&) const;


		// Vector addition
		Vector2 operator+(const Vector2&) const;

		// Vector subtraction
		Vector2 operator-(const Vector2&) const;


		// Vector Augmented assignment addition
		void operator+=(const Vector2&);

		// Vector Augmented assignment subtraction
		void operator-=(const Vector2&);


		// Vector multiplication by a scalar
		Vector2 operator*(const float&) const;

		// Vector multiplication by a vector
		Vector2 operator*(const Vector2&) const;

		// Vector division by a scalar
		Vector2 operator/(const float&) const;

		// Vector division by a vector
		Vector2 operator/(const Vector2&) const;


		// Vector augmented assignment division by a scalar
		void operator*=(const float&);

		// Vector augmented assignment division by a scalar
		void operator*=(const Vector2&);

		// Vector augmented assignment division by a scalar
		void operator/=(const float&);

		// Vector augmented assignment division by a scalar
		void operator/=(const Vector2&);


		// Dot product by another vector
		float dot(const Vector2& vec) const;

		float length() const;

		void normalize();
		Vector2 normalized();

		// Calculate scalar projection of this vector on b
		float scalar_projection(const Vector2& b) const;
		// Calculate vector projection of this vector on b
		Vector2 vector_projection(Vector2 b) const;
		// Calculate vector projection of this vector on b given an already found scalar projection
		Vector2 vector_projection(Vector2 b, const float scalar_proj) const;
		// Calculate vector rejection of this vector on b
		Vector2 vector_rejection(const Vector2& b) const;
		// Calculate vector rejection given of this vector on b an already found scalar projection
		Vector2 vector_rejection(const Vector2 & b, const float scalar_proj) const;

		// the angle from this vector to the other vector
		//NOTE: both vectors need to be normalized
		float angle(const Vector2& vec) const;

		// // serialize
		// template <class Archive>
		// void serialize(Archive & ar)
		// {
		// 	ar(CEREAL_NVP(x),
		// 	   CEREAL_NVP(y));
		// }
	};
}
