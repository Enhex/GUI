#include "double_clickable.h"

using namespace std::chrono;

namespace gui
{

double_clickable::double_clickable()
{
	// initialize with a time point furthest away from now()
	// as a way to tell if a time press was stored yet or not.
	press_time[0] = press_time[1] = time_point_t(steady_clock::now() - time_point_t::max());
}

void double_clickable::on_mouse_press(input::element const* el)
{
	press_time_index = !press_time_index;
	press_time[press_time_index] = steady_clock::now();
	pressed_el[press_time_index] = el;
}

bool double_clickable::on_mouse_release(input::element const* el)
{
	// check if both clicks are on the same element
	if(pressed_el[!press_time_index] != el){
		return false;
	}

	auto const time_passed = duration_cast<milliseconds>(steady_clock::now() - press_time[!press_time_index]).count();
	auto const double_click_ready = (time_passed < double_click_time) & (time_passed >= 0);
	return double_click_ready;
}

}
