#include "rectangle.h"

// check if a point is inside the rectangle
bool rectangle::is_inside(vector2 const& point)
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

void rectangle::merge(vector2 const& point)
{
	auto& min = position;
	auto max = position + size;

	X(min) = std::min(X(min), X(point));
	X(max) = std::max(X(max), X(point));
	Y(min) = std::min(Y(min), Y(point));
	Y(max) = std::max(Y(max), Y(point));

	size = max - position;
}

void rectangle::merge(rectangle const& other)
{
	merge(other.position);
	merge(other.position + other.size);
}