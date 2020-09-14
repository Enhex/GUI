#pragma once

#include "../event/base.h"
#include "../math/math.h"
#include <tuple>

namespace input
{
	namespace event
	{
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

		struct key_repeat : base
		{
			static constexpr size_t id = 7;
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

		struct mouse_press : base
		{
			static constexpr size_t id = 4;
			size_t get_id() const { return id; }

			// button, mods
			using params = std::tuple<int, int>;
		};

		struct mouse_release : base
		{
			static constexpr size_t id = 5;
			size_t get_id() const { return id; }

			// button, mods
			using params = std::tuple<int, int>;
		};

		struct scroll : base
		{
			static constexpr size_t id = 6;
			size_t get_id() const { return id; }

			// x offset, y offset
			using params = std::tuple<double, double>;
		};

		struct character : base
		{
			static constexpr size_t id = 7;
			size_t get_id() const { return id; }

			// codepoint
			using params = std::tuple<unsigned int>;
		};

		struct frame_start : base
		{
			static constexpr size_t id = 8;
			size_t get_id() const { return id; }

			// codepoint
			using params = std::tuple<>;
		};
	}
}