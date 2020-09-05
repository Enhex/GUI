#pragma once

#include "panel.h"

struct button : panel
{
	inline static constexpr auto element_name{ "button" };
	std::string get_element_name() override { return element_name; }

	NVGcolor background_color{ 0,0,0,1 };
	NVGcolor hover_color{ 0.5,0.5,0.5,1 };
	NVGcolor press_color{ 1,1,1,1 };

	std::function<void()> callback;

	button();

	void set_style(style::style_t const& style) override;
};