#pragma once

#include "element.h"

#include "../include_glfw.h"
#include "scrollbar.h"

/*TODO
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

	float scroll_step = 50;

	// position-local to the handle, which mouse drag will be aligned with.
	float handle_drag_ancor;

	scroll_view() :
	view(create_child<scissor>()),
	content(view.create_child<element>()),
	scroll(create_child<scrollbar>())
	{
		style = element_name;
		
		create_layout<gui::layout::box>().orient = layout::horizontal;

		// elements
		view.expand = {true, true};

		content.expand = {false, false};
		content.create_layout<gui::layout::shrink>().shrink_position = false;

		scroll.expand = {false, true};

		// input
		auto& input_manager = context->input_manager;
		
		input_manager.subscribe<input::event::mouse_press>(&scroll.scroll_up, [&](std::any&& args) {
			move_content(scroll_step);
		});

		input_manager.subscribe<input::event::mouse_press>(&scroll.scroll_down, [&](std::any&& args) {
			move_content(-scroll_step);
		});

		input_manager.subscribe<input::event::mouse_press>(&scroll.handle, [&](std::any&& args) {
			auto&[button, mods] = std::any_cast<input::event::mouse_press::params&>(args);
			if(button != GLFW_MOUSE_BUTTON_LEFT)
				return;

			// start dragging
			handle_drag_ancor = Y(input_manager.mouse_pos) - Y(scroll.handle.get_position());

			// subscribe to events
			input_manager.subscribe_global<input::event::frame_start>(this, [&](std::any&& args) {
				// update handle position
				auto const new_absolute_pos = Y(input_manager.mouse_pos) - handle_drag_ancor;
				auto const change = Y(scroll.handle.get_position()) - new_absolute_pos;
				if (change != 0)
					move_content(change);
			});

			input_manager.subscribe<input::event::mouse_release>(&scroll.handle, [&](std::any&& args) {
				auto&[button, mods] = std::any_cast<input::event::mouse_press::params&>(args);
				stop_dragging(button);
			});

			input_manager.subscribe_global<input::event::mouse_release>(this, [&](std::any&& args) {
				auto&[button, mods] = std::any_cast<input::event::mouse_press::params&>(args);
				stop_dragging(button);
			});
		});

		//TODO clicking the track should move page up/down
	}

	void stop_dragging(int const button)
	{
		if (button != GLFW_MOUSE_BUTTON_LEFT)
			return;

		// unsubscribe for optimization to not call callbacks needlessly when not dragging
		auto& input_manager = context->input_manager;
		input_manager.unsubscribe<input::event::mouse_release>(&scroll.handle);
		input_manager.unsubscribe_global<input::event::mouse_release>(this);
		input_manager.unsubscribe_global<input::event::frame_start>(this);
	}

	void move_content(float change)
	{
		Y(content.position) += change;
		Y(content.position) = std::clamp(Y(content.position), -Y(content.size), 0.f);
		update_handle_position();
	}

	void update_handle_size()
	{
		Y(scroll.handle.min_size) = Y(scroll.track.size) * std::min(1.f, Y(view.size) / Y(content.size));
		scroll.handle.apply_min_size();
	}

	void update_handle_position()
	{
		auto const content_position_fraction = (Y(content.position)*-1) / Y(content.size);
		auto const handle_position_size = Y(scroll.track.size) - Y(scroll.handle.size); // handle need to be stopped before it goes outside the track
		Y(scroll.handle.position) = handle_position_size * content_position_fraction;
	}

	void post_layout() override
	{
		update_handle_size();
	}
};