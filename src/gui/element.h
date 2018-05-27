#pragma once

#include "../input/input.h"
#include "../layout/layout.h"
#include <nanovg.h>

struct element : layout::element<element>, input::element
{
	virtual void draw(NVGcontext* vg) {}
	void draw_recursive(NVGcontext* vg) {
		draw(vg);
		for (auto const& child : children)
			child->draw_recursive(vg);
	}
};

namespace gui::layout
{
	using box = ::layout::box<::element>;
}