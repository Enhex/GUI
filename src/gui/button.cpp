#include "button.h"

button::button()
{
	color = background_color;

	style = element_name;
	apply_style();

	create_layout<gui::layout::box>();

	enable();
}

void button::enable()
{
	auto& input_manager = context->input_manager;

	input_manager.hover_start.subscribe(this, [this](nx::Vector2 const& mouse_position) {
		color = hover_color;
	});

	input_manager.hover_end.subscribe(this, [this]() {
		if(is_focused())
			color = focus_color;
		else
			color = background_color;
	});

	input_manager.mouse_press.subscribe(this, [this](int key, int mods) {
		color = press_color;
		is_pressed = true;
	});

	input_manager.double_click.subscribe(this, [this](int key, int mods) {
		color = hover_color;
		//execute callback if the button was pressed before release event
		if (is_pressed) {
			is_pressed = false;
			if(double_click_callback){
				double_click_callback();
			}
		}
	});

	input_manager.mouse_release.subscribe(this, [this](int key, int mods) {
		color = hover_color;
		//execute callback if the button was pressed before release event
		if (is_pressed) {
			is_pressed = false;
			callback();
		}
	});

	input_manager.focus_start.subscribe(this, [this]() {
		color = focus_color;
	});
	input_manager.focus_end.subscribe(this, [this]() {
		if(is_hovered())
			color = hover_color;
		else
			color = background_color;
	});
}

void button::disable()
{
	auto& input_manager = context->input_manager;

	input_manager.hover_start.unsubscribe(this);
	input_manager.hover_end.unsubscribe(this);
	input_manager.mouse_press.unsubscribe(this);
	// unsubscrubing from mouse_release causes a crash
	// input_manager.mouse_release.unsubscribe(this);
	input_manager.focus_start.unsubscribe(this);
	input_manager.focus_end.unsubscribe(this);
}


void button::set_style(style::style_t const& style)
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