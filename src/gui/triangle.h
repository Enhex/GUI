#pragma once

#include "element.h"

#include "../math/Vector2.h"

struct triangle : element
{
	inline static constexpr auto element_name{ "triangle" };
	std::string get_element_name() noexcept override { return element_name; }

	std::type_info const& type_info() const noexcept override { return typeid(triangle); }

	NVGcolor color{0,0,0,1};
	std::array<nx::Vector2, 3> vertices;

	triangle();

	void set_style(style::style_t const& style) override;

	void draw(NVGcontext* vg) override;
};