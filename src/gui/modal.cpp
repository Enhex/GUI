#include "modal.h"

modal::modal()
{
	style = element_name;
	color = NVGcolor{0,0,0,0.75};

	expand = {true, true};
	visible = false;

	auto& input_manager = context->input_manager;

	input_manager.subscribe<input::event::mouse_press>(this, [this](std::any&& args) {
		is_pressed = true;
	});

	input_manager.subscribe<input::event::mouse_release>(this, [this](std::any&& args) {
		if (is_pressed) {
			is_pressed = false;
			visible = false;
		}
	});
}