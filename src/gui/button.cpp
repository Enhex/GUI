#include "button.h"

button::button()
{
	// initialize with a time point furthest away from now()
	// as a way to tell if a time press was stored yet or not.
	press_time[0] = press_time[1] = time_point_t(std::chrono::steady_clock::now() - time_point_t::max());

	color = background_color;

	style = element_name;
	apply_style();

	create_layout<gui::layout::box>();

	enable();
}

void button::enable()
{
	auto& input_manager = context->input_manager;

	input_manager.hover_start.subscribe(this, [this](vector2 const& mouse_position) {
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
		press_time_index = !press_time_index;
		press_time[press_time_index] = std::chrono::steady_clock::now();
	});

	input_manager.mouse_release.subscribe(this, [this](int key, int mods) {
		color = hover_color;
		//execute callback if the button was pressed before release event
		if (is_pressed) {
			is_pressed = false;
			auto const time_passed = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now() - press_time[!press_time_index]).count();
			auto const double_click_ready = (time_passed < double_click_time) & (time_passed >= 0);
			if(double_click_ready &&
				double_click_callback)
			{
				double_click_callback();
			}
			else if(callback)
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