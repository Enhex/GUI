#include "rectangle.h"

// check if a point is inside the rectangle
bool nx::rectangle::is_inside(nx::Vector2 const& point)
{
	auto& min = position;
	auto max = position + size;

	if (point.x >= min.x &&
		point.y >= min.y &&
		point.x <= max.x &&
		point.y <= max.y)
		return true;
	return false;
}

void nx::rectangle::merge(nx::Vector2 const& point)
{
	auto& min = position;
	auto max = position + size;

	min.x = std::min(min.x, point.x);
	max.x = std::max(max.x, point.x);
	min.y = std::min(min.y, point.y);
	max.y = std::max(max.y, point.y);

	size = max - position;
}

void nx::rectangle::merge(nx::rectangle const& other)
{
	merge(other.position);
	merge(other.position + other.size);
}