#include "double_clickable.h"

double_clickable::double_clickable()
{
	// initialize with a time point furthest away from now()
	// as a way to tell if a time press was stored yet or not.
	press_time[0] = press_time[1] = time_point_t(std::chrono::steady_clock::now() - time_point_t::max());
}

void double_clickable::on_mouse_press()
{
	press_time_index = !press_time_index;
	press_time[press_time_index] = std::chrono::steady_clock::now();
}

bool double_clickable::on_mouse_release()
{
	using namespace std::chrono;
	auto const time_passed = duration_cast<milliseconds>(steady_clock::now() - press_time[!press_time_index]).count();
	auto const double_click_ready = (time_passed < double_click_time) & (time_passed >= 0);
	return double_click_ready;
}
