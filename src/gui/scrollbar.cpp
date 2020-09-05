#include "scrollbar.h"

#include "button.h"

scrollbar::scrollbar() :
	scroll_up(create_child<button>()),
	track(create_child<button>()),
	handle(track.create_child<button>()),
	scroll_down(create_child<button>())
{
	style = element_name;

	expand = {false, true};
	create_layout<gui::layout::box>();

	auto color_scroll_button = [&](button& el) {
		el.color = el.background_color = nvgRGBA(80, 80, 80, 255);
		el.hover_color = nvgRGBA(120, 120, 120, 255);
		el.press_color = nvgRGBA(0, 120, 210, 255);
	};

	scroll_up.min_size = { 20,20 };
	color_scroll_button(scroll_up);

	track.min_size = { 20,40 };
	track.color = track.background_color = nvgRGBA(50, 50, 50, 255);
	track.hover_color = nvgRGBA(50, 50, 50, 255);
	track.press_color = nvgRGBA(50, 50, 50, 255);
	track.expand = {false, true};
	track.create_layout<gui::layout::forward>();

	handle.min_size = { 20,20 };
	handle.color = handle.background_color = nvgRGBA(100, 100, 100, 255);
	handle.hover_color = nvgRGBA(120, 120, 120, 255);
	handle.press_color = nvgRGBA(210, 210, 210, 255);

	scroll_down.min_size = { 20,20 };
	color_scroll_button(scroll_down);

	/*TODO
	expand depends on the scrollbar's orientation
	
	create handle
		- input events for dragging
		- ability to set size
			- min size

	TODO arrow buttons
	*/
}