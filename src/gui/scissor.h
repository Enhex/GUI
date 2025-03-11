#pragma once

#include "element.h"

struct scissor : element
{
	inline static constexpr auto element_name{ "scissor" };
	std::string get_element_name() noexcept override { return element_name; }

	std::type_info const& type_info() const noexcept override { return typeid(scissor); }

	// scissors child elements
	void draw_recursive(NVGcontext* vg) override;
};