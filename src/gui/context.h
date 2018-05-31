#pragma once

#include "../input/manager.h"
#include "../style/style.h"
#include <nanovg.h>

namespace gui
{
	struct context
	{
		input::manager input_manager;
		style::manager style_manager;
		NVGcontext* vg = nullptr;
	};
}