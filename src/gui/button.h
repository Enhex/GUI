#pragma once

#include "panel.h"

#include <chrono>

struct button : panel
{
	inline static constexpr auto element_name{ "button" };
	std::string get_element_name() override { return element_name; }

	NVGcolor background_color{ 0.1,0.1,0.1,1 };
	NVGcolor hover_color{ 0.32,0.32,0.32,1 };
	NVGcolor press_color{ 0.25,0.25,0.25,1 };
	NVGcolor focus_color{ 0.3,0.3,0.3,1 };

	std::function<void()> callback;
	std::function<void()> double_click_callback;

	// how long to wait in milliseconds before considering the next mouse release a double click
	int_fast16_t double_click_time = 500;

	button();

	void set_style(style::style_t const& style) override;

	void enable();
	void disable();

protected:
	bool is_pressed = false;
	using time_point_t = std::chrono::time_point<std::chrono::steady_clock>;
	std::array<time_point_t, 2> press_time;
	uint_fast8_t press_time_index = 0;
};