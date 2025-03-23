#include "scrollbar.h"

#include "button.h"
#include "triangle.h"

static auto const arrow_color = nvgRGBA(135, 135, 135, 255);

scrollbar::scrollbar(layout::orientation const orient) :
	scroll_up(create_child<button>()),
	track(create_child<button>()),
	handle(track.create_child<button>()),
	scroll_down(create_child<button>())
{
	style = element_name;

	expand = {false, true};
	create_layout<gui::layout::box>().orient = orient;

	auto color_scroll_button = [&](button& el) {
		el.color = el.background_color = nvgRGBA(80, 80, 80, 255);
		el.hover_color = nvgRGBA(120, 120, 120, 255);
		el.press_color = nvgRGBA(0, 120, 210, 255);
	};

	scroll_up.min_size = { 20,20 };
	color_scroll_button(scroll_up);
	{
		auto& arrow = scroll_up.create_child<triangle>();
		arrow.min_size = { 20,20 };
		arrow.color = arrow_color;
		if(orient == layout::orientation::vertical){
			// up
			arrow.vertices[0].x = arrow.min_size.x / 2;
			arrow.vertices[0].y = arrow.min_size.y / 3;
			arrow.vertices[1].x = arrow.min_size.x / 4;
			arrow.vertices[1].y = (arrow.min_size.y / 3) * 2;
			arrow.vertices[2].x = (arrow.min_size.x / 4) * 3;
			arrow.vertices[2].y = (arrow.min_size.y / 3) * 2;
		}
		else{
			// left
			arrow.vertices[0].x = arrow.min_size.x / 3;
			arrow.vertices[0].y = arrow.min_size.y / 2;
			arrow.vertices[1].x = (arrow.min_size.x / 3) * 2;
			arrow.vertices[1].y = arrow.min_size.y / 4;
			arrow.vertices[2].x = (arrow.min_size.x / 3) * 2;
			arrow.vertices[2].y = (arrow.min_size.y / 4) * 3;
		}
	}

	track.min_size = { 20,20 };
	track.color = track.background_color = nvgRGBA(50, 50, 50, 255);
	track.hover_color = nvgRGBA(50, 50, 50, 255);
	track.press_color = nvgRGBA(50, 50, 50, 255);
	track.expand[orient] = true;
	track.create_layout<gui::layout::forward>();

	handle.min_size = { 20,20 };
	handle.color = handle.background_color = nvgRGBA(100, 100, 100, 255);
	handle.hover_color = nvgRGBA(120, 120, 120, 255);
	handle.press_color = nvgRGBA(210, 210, 210, 255);

	scroll_down.min_size = { 20,20 };
	color_scroll_button(scroll_down);
	{
		auto& arrow = scroll_down.create_child<triangle>();
		arrow.min_size = { 20,20 };
		arrow.color = arrow_color;
		if(orient == layout::orientation::vertical){
			// down
			arrow.vertices[0].x = arrow.min_size.x / 2;
			arrow.vertices[0].y = (arrow.min_size.y / 3) * 2;
			arrow.vertices[1].x = arrow.min_size.x / 4;
			arrow.vertices[1].y = arrow.min_size.y / 3;
			arrow.vertices[2].x = (arrow.min_size.x / 4) * 3;
			arrow.vertices[2].y = arrow.min_size.y / 3;
		}
		else{
			// right
			arrow.vertices[0].x = (arrow.min_size.x / 3) * 2;
			arrow.vertices[0].y = arrow.min_size.y / 2;
			arrow.vertices[1].x = arrow.min_size.x / 3;
			arrow.vertices[1].y = arrow.min_size.y / 4;
			arrow.vertices[2].x = arrow.min_size.x / 3;
			arrow.vertices[2].y = (arrow.min_size.y / 4) * 3;
		}
	}

	/*TODO
	expand depends on the scrollbar's orientation

	create handle
		- input events for dragging
		- ability to set size
			- min size

	TODO arrow buttons
	*/
}