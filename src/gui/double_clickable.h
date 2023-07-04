#pragma once

#include "panel.h"

#include <chrono>

struct double_clickable
{
	std::function<void()> double_click_callback;

	// how long to wait in milliseconds before considering the next mouse release a double click
	int_fast16_t double_click_time = 500;

	double_clickable();

	void on_mouse_press();
	bool on_mouse_release();

protected:
	bool is_pressed = false;
	using time_point_t = std::chrono::time_point<std::chrono::steady_clock>;
	std::array<time_point_t, 2> press_time;
	uint_fast8_t press_time_index = 0;
};