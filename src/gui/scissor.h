#pragma once

#include "element.h"

struct scissor : element
{
	inline static constexpr auto element_name{ "scissor" };
	std::string get_element_name() override { return element_name; }

	std::type_info const& type_info() const override { return typeid(scissor); }

	// scissors child elements
	virtual void draw_recursive(NVGcontext* vg) override 
	{
		nvgSave(vg);
		nvgIntersectScissor(vg, X(position), Y(position), X(size), Y(size));

		for (auto const& child : children)
			child->draw_recursive(vg);

		// using save and restore to handle nested scissor elements, so one a child scissor is done it will restore its parent's scissor.
		nvgRestore(vg);
	}
};