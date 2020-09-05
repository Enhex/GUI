#pragma once

#include "vector.h"

struct rectangle
{
	vector2 position{ 0,0 };
	vector2 size{ 0,0 };

	// check if a point is inside the rectangle
	bool is_inside(vector2 const& point);

	void merge(vector2 const& point);

	// merge with another rectangle into a new rectangle that fits around both
	void merge(rectangle const& other);
};