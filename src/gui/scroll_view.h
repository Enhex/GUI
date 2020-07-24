#pragma once

#include "element.h"

#include "scrollbar.h"

/*TODO
- set scrollbar handle size relative to size/overflow
- hold middle mouse click to scroll in all directions
- virtual create_child(), to add to the content scissor
*/
struct scroll_view : element
{
	inline static constexpr auto element_name{ "scroll_view" };
	std::string get_element_name() override { return element_name; }

	std::type_info const& type_info() const override { return typeid(scroll_view); }

	scissor& view;
	element& content; // used to move all the elements inside the view
	scrollbar& scroll;

	float scroll_step = 10;

	scroll_view() :
	view(create_child<scissor>()),
	content(view.create_child<element>()),
	scroll(create_child<scrollbar>())
	{
		style = element_name;
		
		auto& box = create_layout<gui::layout::box>();
		box.orient = layout::horizontal;

		// elements
		view.expand = {true, true};
		auto& forward = view.create_layout<gui::layout::forward>();

		content.expand = {false, false};
		auto& shrink = content.create_layout<gui::layout::shrink>();

		scroll.expand = {false, true};

		// input
		auto& input_manager = context->input_manager;
		
		input_manager.subscribe<input::event::mouse_press>(&scroll.scroll_up, [&](std::any&& args) {
			auto&[button, mods] = std::any_cast<input::event::mouse_press::params&>(args);
			// move content
			boost::qvm::Y(content.position) += scroll_step;
		});

		input_manager.subscribe<input::event::mouse_press>(&scroll.scroll_down, [&](std::any&& args) {
			auto&[button, mods] = std::any_cast<input::event::mouse_press::params&>(args);
			// move content
			boost::qvm::Y(content.position) -= scroll_step;
		});
	}

	void update_handle_size()
	{
		Y(scroll.handle.min_size) = Y(scroll.track.size) * std::min(1.f, Y(view.size) / Y(content.size));
		scroll.handle.apply_min_size();
	}
};