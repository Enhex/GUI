#pragma once

#include <chrono>

namespace input{
	struct element;
}

struct double_clickable
{
	// how long to wait in milliseconds before considering the next mouse release a double click
	int_fast16_t double_click_time = 500;

	double_clickable();

	void on_mouse_press(input::element const* el);
	bool on_mouse_release(input::element const* el);

protected:
	using time_point_t = std::chrono::time_point<std::chrono::steady_clock>;
	std::array<time_point_t, 2> press_time;
	std::array<input::element const*, 2> pressed_el{nullptr, nullptr}; // pressed element
	uint_fast8_t press_time_index = 0;
};