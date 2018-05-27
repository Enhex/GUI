#pragma once

#include <limits>

namespace input
{
	namespace event
	{
		auto static constexpr invalid_id = std::numeric_limits<size_t>::max();

		struct base
		{
			virtual size_t get_id() = 0;
			//TODO name could be useful, especially for interpreting events from data files
		};
	}
}