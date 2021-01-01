#include "scroll_view.h"

#include "../include_glfw.h"
#include "button.h"
#include "scissor.h"
#include "scrollbar.h"

//NOTE: using namespace layout causes name collision between layout::element and gui::element
constexpr auto vertical = layout::vertical;
constexpr auto horizontal = layout::horizontal;

scroll_view::scroll_view() :
	vertical_container(create_child<element>()),
	view(vertical_container.create_child<scissor>()),
	content(view.create_child<element>()),
	scroll({&vertical_container.create_child<scrollbar>(horizontal),
	       &create_child<scrollbar>(vertical)})
{
	style = element_name;

	create_layout<gui::layout::box>().orient = layout::horizontal;

	// elements
	vertical_container.create_layout<gui::layout::box>();
	vertical_container.expand = {true, true};

	view.expand = {true, true};

	content.expand = {false, false};
	content.create_layout<gui::layout::shrink>().shrink_position = false;

	scroll[horizontal]->expand = {true, false};
	scroll[vertical]->expand = {false, true};

	// input
	auto& input_manager = context->input_manager;

	input_manager.subscribe<input::event::scroll>(this, [&](std::any&& args) {
		auto&[xoffset, yoffset] = std::any_cast<input::event::scroll::params&>(args);
		move_content(horizontal, scroll_step * xoffset);
		move_content(vertical, scroll_step * yoffset);
	});

	subscribe_scroll_events(horizontal);
	subscribe_scroll_events(vertical);
}

void scroll_view::subscribe_scroll_events(layout::orientation const orient)
{
	auto& input_manager = context->input_manager;

	input_manager.subscribe<input::event::mouse_press>(&scroll[orient]->scroll_up, [&, orient](std::any&& args) {
		move_content(orient, scroll_step);
	});

	input_manager.subscribe<input::event::mouse_press>(&scroll[orient]->scroll_down, [&, orient](std::any&& args) {
		move_content(orient, -scroll_step);
	});

	input_manager.subscribe<input::event::mouse_press>(&scroll[orient]->handle, [&, orient](std::any&& args) {
		auto&[button, mods] = std::any_cast<input::event::mouse_press::params&>(args);
		if(button != GLFW_MOUSE_BUTTON_LEFT)
			return;

		// start dragging
		handle_drag_ancor = input_manager.mouse_pos.a[orient] - scroll[orient]->handle.get_position().a[orient];

		// subscribe to events
		input_manager.subscribe_global<input::event::frame_start>(this, [&, orient](std::any&& args) {
			// update handle position
			auto const new_absolute_pos = input_manager.mouse_pos.a[orient] - handle_drag_ancor;
			auto const change = scroll[orient]->handle.get_position().a[orient] - new_absolute_pos;
			if (change != 0)
				move_content(orient, change);
		});

		input_manager.subscribe<input::event::mouse_release>(&scroll[orient]->handle, [&, orient](std::any&& args) {
			auto&[button, mods] = std::any_cast<input::event::mouse_press::params&>(args);
			stop_dragging(orient, button);
		});

		input_manager.subscribe_exclusive<input::event::mouse_release>([&, orient](std::any&& args) {
			auto&[button, mods] = std::any_cast<input::event::mouse_press::params&>(args);
			stop_dragging(orient, button);
		});
	});

	//TODO clicking the track should move page up/down
}

void scroll_view::stop_dragging(layout::orientation const orient, int const button)
{
	if (button != GLFW_MOUSE_BUTTON_LEFT)
		return;

	// unsubscribe for optimization to not call callbacks needlessly when not dragging
	auto& input_manager = context->input_manager;
	input_manager.unsubscribe<input::event::mouse_release>(&scroll[orient]->handle);
	input_manager.unsubscribe_exclusive<input::event::mouse_release>();
	input_manager.unsubscribe_global<input::event::frame_start>(this);
}

float scroll_view::get_scroll_length(layout::orientation const orient) const
{
	return content.size.a[orient] - view.size.a[orient];
}

void scroll_view::move_content(layout::orientation const orient, float change)
{
	if(content.size.a[orient] < view.size.a[orient])
		return;

	content.position.a[orient] += change;
	content.position.a[orient] = std::clamp(content.position.a[orient], -get_scroll_length(orient), 0.f);
	update_handle_position(orient);
}

void scroll_view::update_handle_size(layout::orientation const orient)
{
	auto& handle = scroll[orient]->handle;
	handle.size.a[orient] = handle.min_size.a[orient] = scroll[orient]->track.size.a[orient] * std::min(1.f, view.size.a[orient] / content.size.a[orient]);
}

void scroll_view::update_handle_position(layout::orientation const orient)
{
	auto const scroll_length = get_scroll_length(orient);
	if(scroll_length > 0) {
		auto const content_position_fraction = (content.position.a[orient] * -1) / scroll_length;
		auto const handle_position_size = scroll[orient]->track.size.a[orient] - scroll[orient]->handle.size.a[orient]; // handle need to be stopped before it goes outside the track
		scroll[orient]->handle.position.a[orient] = handle_position_size * content_position_fraction;
	}
	else {
		scroll[orient]->handle.position.a[orient] = 0.f; // no space for moving the handle, so keep it at the start
	}
}

void scroll_view::post_layout()
{
	update_handle_size(horizontal);
	update_handle_size(vertical);
}