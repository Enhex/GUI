#pragma once

#include "element.h"

struct panel : element
{
	inline static constexpr auto element_name{ "panel" };
	std::string get_element_name() noexcept override { return element_name; }

	std::type_info const& type_info() const noexcept override { return typeid(panel); }

	NVGcolor color{0,0,0,1};

	panel();

	void set_style(style::style_t const& style) override;

	void draw(NVGcontext* vg) override;
};