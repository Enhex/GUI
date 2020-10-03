#pragma once

#include "panel.h"

struct button : panel
{
	inline static constexpr auto element_name{ "button" };
	std::string get_element_name() override { return element_name; }

	NVGcolor background_color{ 0.1,0.1,0.1,1 };
	NVGcolor hover_color{ 0.3,0.3,0.3,1 };
	NVGcolor press_color{ 0.25,0.25,0.25,1 };

	std::function<void()> callback;

	button();

	void set_style(style::style_t const& style) override;

protected:
	bool is_pressed = false;
};