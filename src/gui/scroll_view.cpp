#include "scroll_view.h"

#include "../include_glfw.h"
#include "button.h"
#include "scissor.h"
#include "scrollbar.h"

scroll_view::scroll_view() :
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

void scroll_view::stop_dragging(int const button)
{
	if (button != GLFW_MOUSE_BUTTON_LEFT)
		return;

	// unsubscribe for optimization to not call callbacks needlessly when not dragging
	auto& input_manager = context->input_manager;
	input_manager.unsubscribe<input::event::mouse_release>(&scroll.handle);
	input_manager.unsubscribe_global<input::event::mouse_release>(this);
	input_manager.unsubscribe_global<input::event::frame_start>(this);
}

float scroll_view::get_scroll_length() const
{
	return Y(content.size) - Y(view.size);
}

void scroll_view::move_content(float change)
{
	if(Y(content.size) < Y(view.size))
		return;

	Y(content.position) += change;
	Y(content.position) = std::clamp(Y(content.position), -get_scroll_length(), 0.f);
	update_handle_position();
}

void scroll_view::update_handle_size()
{
	Y(scroll.handle.min_size) = Y(scroll.track.size) * std::min(1.f, Y(view.size) / Y(content.size));
	scroll.handle.apply_min_size();
}

void scroll_view::update_handle_position()
{
	auto const content_position_fraction = (Y(content.position)*-1) / get_scroll_length();
	auto const handle_position_size = Y(scroll.track.size) - Y(scroll.handle.size); // handle need to be stopped before it goes outside the track
	Y(scroll.handle.position) = handle_position_size * content_position_fraction;
}

void scroll_view::post_layout()
{
	update_handle_size();
}