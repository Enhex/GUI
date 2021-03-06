#pragma once

#include "../event/base.h"
#include "../math/math.h"

namespace input
{
	namespace event
	{
		// args: key, mods
		struct key_press : base<int,int>{};
		// args: key, mods
		struct key_release : base<int,int>{};
		// args: key, mods
		struct key_repeat : base<int,int>{};
		// args: mouse_position
		struct hover_start : base<vector2 const&>{};
		struct hover_end : base<>{};
		// args: button, mods
		struct mouse_press : base<int,int>{};
		// args: button, mods
		struct mouse_release : base<int,int>{};
		// args: x offset, y offset
		struct scroll : base<double,double>{};
		// args: codepoint
		struct character : base<unsigned int>{};

		struct frame_start : base<>{};
		struct focus_start : base<>{};
		struct focus_end : base<>{};
	}
}