#include "modal.h"

namespace gui
{

modal::modal()
{
	style = element_name;
	color = NVGcolor{0,0,0,0.75};

	expand = {true, true};
	visible = false;

	auto& input_manager = context->input_manager;

	input_manager.mouse_press.subscribe(this, [this](int button, int mods) {
		is_pressed = true;
	});

	input_manager.mouse_release.subscribe(this, [this](int button, int mods) {
		if (is_pressed) {
			is_pressed = false;
			visible = false;
		}
	});
}

}
