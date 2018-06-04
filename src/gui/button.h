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
		auto iter = context->style_manager.styles.find("button");
		if (iter != context->style_manager.styles.end()) {
			set_style(iter->second);
		}

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


	void set_style(style::style_t const& style)
	{
		auto read = [&](auto& property, std::string&& name)
		{
			using property_t = std::remove_reference_t<decltype(property)>;

			auto iter = style.find(name);
			if (iter != style.end()) {
				property = std::any_cast<property_t>(iter->second);
				return true;
			}
			return false;
		};

		if (read(background_color, "background_color")) {
			color = background_color;
		}

		read(hover_color, "hover_color");
		read(press_color, "press_color");
	}
};