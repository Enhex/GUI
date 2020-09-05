#pragma once

#include "element.h"

struct button;

struct scrollbar : element
{
	inline static constexpr auto element_name{ "scrollbar" };
	std::string get_element_name() override { return element_name; }

	std::type_info const& type_info() const override { return typeid(scrollbar); }

	button& scroll_up;
	button& track;
	button& handle;
	button& scroll_down;

	scrollbar();
};