#pragma once

#include "panel.h"

// background that listens to clicks, usually to close a popup menu.
struct modal : panel
{
	inline static constexpr auto element_name{ "modal" };
	std::string get_element_name() override { return element_name; }

	std::type_info const& type_info() const override { return typeid(panel); }

	modal();

protected:
	bool is_pressed = false;
};