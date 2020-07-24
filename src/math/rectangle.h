#pragma once

#include "vector.h"

struct rectangle
{
	vector2 position{ 0,0 };
	vector2 size{ 0,0 };

	// check if a point is inside the rectangle
	bool is_inside(vector2 const& point)
	{
		auto& min = position;
		auto max = position + size;

		if (X(point) >= X(min) &&
			Y(point) >= Y(min) &&
			X(point) <= X(max) &&
			Y(point) <= Y(max))
			return true;
		return false;
	}

	void merge(vector2 const& point)
	{
		auto& min = position;
		auto max = position + size;

		X(min) = std::min(X(min), X(point));
		X(max) = std::max(X(max), X(point));
		Y(min) = std::min(Y(min), Y(point));
		Y(max) = std::max(Y(max), Y(point));

		size = max - position;
	}

	// merge with another rectangle into a new rectangle that fits around both
	void merge(rectangle const& other)
	{
		merge(other.position);
		merge(other.position + other.size);
	}
};