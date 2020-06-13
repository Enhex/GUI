#pragma once

#include "element.h"

#include "button.h"

struct scrollbar : element
{
	inline static constexpr auto element_name{ "scrollbar" };
	std::string get_element_name() override { return element_name; }

	std::type_info const& type_info() const override { return typeid(scrollbar); }


	scrollbar()
	{
		style = element_name;

		expand = {false, true};
		create_layout<gui::layout::box>();

		auto color_scroll_button = [&](button& el) {
			el.color = el.background_color = nvgRGBA(80, 80, 80, 255);
			el.hover_color = nvgRGBA(120, 120, 120, 255);
			el.press_color = nvgRGBA(0, 120, 210, 255);
		};

		auto& scroll_up = create_child<button>();
		scroll_up.min_size = { 20,20 };
		color_scroll_button(scroll_up);

		auto& track = create_child<button>();
		track.min_size = { 20,40 };
		track.color = track.background_color = nvgRGBA(50, 50, 50, 255);
		track.hover_color = nvgRGBA(50, 50, 50, 255);
		track.press_color = nvgRGBA(50, 50, 50, 255);
		track.expand = {false, true};

		auto& handle = track.create_child<button>();
		handle.min_size = { 20,20 };
		handle.color = handle.background_color = nvgRGBA(100, 100, 100, 255);
		handle.hover_color = nvgRGBA(120, 120, 120, 255);
		handle.press_color = nvgRGBA(210, 210, 210, 255);

		auto& scroll_down = create_child<button>();
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
};