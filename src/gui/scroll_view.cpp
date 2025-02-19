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

	input_manager.scroll.subscribe(this, [&](double xoffset, double yoffset) {
		move_content(horizontal, scroll_step * xoffset);
		if(input_manager.key_pressed(GLFW_KEY_LEFT_SHIFT)){
			move_content(horizontal, scroll_step * yoffset);
		}
		else{
			move_content(vertical, scroll_step * yoffset);
		}
	});

	subscribe_scroll_events(horizontal);
	subscribe_scroll_events(vertical);
}

void scroll_view::subscribe_scroll_events(layout::orientation const orient)
{
	auto& input_manager = context->input_manager;

	input_manager.mouse_press.subscribe(&scroll[orient]->scroll_up, [&, orient](int button, int mods) {
		move_content(orient, scroll_step);
	});

	input_manager.mouse_press.subscribe(&scroll[orient]->scroll_down, [&, orient](int button, int mods) {
		move_content(orient, -scroll_step);
	});

	input_manager.mouse_press.subscribe(&scroll[orient]->handle, [&, orient](int button, int mods) {
		if(button != GLFW_MOUSE_BUTTON_LEFT)
			return;

		// start dragging
		handle_drag_ancor = input_manager.mouse_pos[orient] - scroll[orient]->handle.get_position()[orient];

		// subscribe to events
		input_manager.frame_start.subscribe_global(this, [&, orient]() {
			// update handle position
			auto const new_absolute_pos = input_manager.mouse_pos[orient] - handle_drag_ancor;
			auto const change = scroll[orient]->handle.get_position()[orient] - new_absolute_pos;
			if (change != 0)
				move_content(orient, change);
		});

		input_manager.mouse_release.subscribe(&scroll[orient]->handle, [&, orient](int button, int mods) {
			stop_dragging(orient, button);
		});

		input_manager.mouse_release.subscribe_exclusive([&, orient](int button, int mods) {
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
	input_manager.mouse_release.unsubscribe(&scroll[orient]->handle);
	input_manager.mouse_release.unsubscribe_exclusive();
	input_manager.frame_start.unsubscribe_global(this);
}

float scroll_view::get_scroll_length(layout::orientation const orient) const
{
	return content.size[orient] - view.size[orient];
}

void scroll_view::move_content(layout::orientation const orient, float change)
{
	if(content.size[orient] < view.size[orient]) {
		content.position[orient] = 0;
	}
	else {
		content.position[orient] += change;
		content.position[orient] = std::clamp(content.position[orient], -get_scroll_length(orient), 0.f);
	}

	update_handle_position(orient);
}

void scroll_view::scroll_to(layout::orientation const orient, float pos)
{
	auto const view_bottom_pos = view.size[orient] - content.position[orient];
	if(pos > view_bottom_pos){
		auto const scroll_amount = pos - view_bottom_pos - content.position[orient];
		move_content(orient, -scroll_amount);
	}
	else{
		auto const view_top_pos = -content.position[orient];
		if(pos < view_top_pos){
		auto const scroll_amount = view_top_pos - pos;
			move_content(orient, scroll_amount);
		}
	}
}

void scroll_view::scroll_to_top()
{
	content.position[layout::vertical] = 0;
}

void scroll_view::scroll_to_bottom()
{
	content.position[layout::vertical] = -get_scroll_length(layout::vertical);
}

void scroll_view::update_handle_size(layout::orientation const orient)
{
	auto& handle = scroll[orient]->handle;
	auto& track = scroll[orient]->track;
	handle.size[orient] = handle.min_size[orient] = track.size[orient] * std::min(1.f, view.size[orient] / content.size[orient]);
	//if size extrudes past the track move it backwards
	auto const handle_end = handle.size[orient] + handle.position[orient];
	if(handle_end > track.size[orient]) {
		auto const offset = handle_end - track.size[orient];
		move_content(orient, offset);
	}
}

void scroll_view::update_handle_position(layout::orientation const orient)
{
	auto const scroll_length = get_scroll_length(orient);
	if(scroll_length > 0) {
		auto const content_position_fraction = (content.position[orient] * -1) / scroll_length;
		auto const handle_position_size = scroll[orient]->track.size[orient] - scroll[orient]->handle.size[orient]; // handle need to be stopped before it goes outside the track
		scroll[orient]->handle.position[orient] = handle_position_size * content_position_fraction;
	}
	else {
		scroll[orient]->handle.position[orient] = 0.f; // no space for moving the handle, so keep it at the start
	}
}

void scroll_view::post_layout()
{
	update_handle_size(horizontal);
	update_handle_size(vertical);
}