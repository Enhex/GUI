#pragma once

#include "panel.h"
#include "text.h"

struct button : panel
{
	NVGcolor background_color;
	NVGcolor hover_color;
	NVGcolor press_color;
	text& label = create_child<text>();

	std::function<void()> callback;

	button()
	{
		create_layout<gui::layout::box>();

		auto& input_manager = context->input_manager;

		input_manager.subscribe<input::event::hover_start>(&label, [this](std::any&& args) {
			color = hover_color;
		});

		input_manager.subscribe<input::event::hover_end>(&label, [this](std::any&& args) {
			color = background_color;
		});

		input_manager.subscribe<input::event::mouse_press>(&label, [this](std::any&& args) {
			color = press_color;
		});

		input_manager.subscribe<input::event::mouse_release>(&label, [this, &input_manager](std::any&& args) {
			color = hover_color;
			//execute callback if the button was pressed before release event
			if(input_manager.pressed_element == &this->label)
				callback();
		});
	}
};