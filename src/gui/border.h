#pragma once

#include "element.h"

struct panel;

/*

*/
struct border : element
{
	inline static constexpr auto element_name{ "border" };
	std::string get_element_name() override { return element_name; }

	std::type_info const& type_info() const override { return typeid(border); }


	//NOTE: initialization order is set by declaration order
	panel& top;
	element& middle;
	panel& left;
	element& content;
	panel& right;
	panel& bottom;

	border();

	void set_color(NVGcolor color);
	void set_border_size(float size);
};