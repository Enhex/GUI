#pragma once

#include "vector.h"

struct rectangle
{
	vector2 position{ 0,0 };
	vector2 size{ 0,0 };

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
};