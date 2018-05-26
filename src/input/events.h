#pragma once

#include "../math/math.h"
#include <any>
#include <array>
#include <functional>
#include <limits>
#include <unordered_map>
#include <unordered_set>

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

		struct key_release : base
		{
			static constexpr size_t id = 0;
			size_t get_id() const { return id; }

			// key, mods
			using params = std::tuple<int, int>;
		};

		struct key_press : base
		{
			static constexpr size_t id = 1;
			size_t get_id() const { return id; }

			// key, mods
			using params = std::tuple<int, int>;
		};

		struct hover_start : base
		{
			static constexpr size_t id = 2;
			size_t get_id() const { return id; }

			// mouse_position
			using params = std::tuple<vector2 const&>;
		};

		struct hover_end : base
		{
			static constexpr size_t id = 3;
			size_t get_id() const { return id; }

			// mouse_position
			//using params = std::tuple<vector2 const&>;
		};

		struct mouse_release : base
		{
			static constexpr size_t id = 4;
			size_t get_id() const { return id; }

			// button, mods
			using params = std::tuple<int, int>;
		};

		struct mouse_press : base
		{
			static constexpr size_t id = 5;
			size_t get_id() const { return id; }

			// button, mods
			using params = std::tuple<int, int>;
		};
	}
}