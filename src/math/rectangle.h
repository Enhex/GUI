#pragma once

#include "Vector2.h"

struct rectangle
{
	nx::Vector2 position{ 0,0 };
	nx::Vector2 size{ 0,0 };

	// check if a point is inside the rectangle
	bool is_inside(nx::Vector2 const& point);

	void merge(nx::Vector2 const& point);

	// merge with another rectangle into a new rectangle that fits around both
	void merge(rectangle const& other);
};