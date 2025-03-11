#pragma once

#include <functional>
#include <vector>

//TODO move to a separate file/lib
struct dynamic_destructor
{
	std::list<std::function<void()>> destructor_callbacks;

	virtual ~dynamic_destructor()
	{
		for (auto& callback : destructor_callbacks)
			callback();
	}
};

namespace input
{
	struct element : dynamic_destructor
	{
		virtual element* get_parent() noexcept
		{
			return parent;
		}

	protected:
		element* parent = nullptr;
	};
}