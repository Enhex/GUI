#pragma once

#include "panel.h"

namespace gui
{

struct button : panel
{
	inline static constexpr auto element_name{ "button" };
	std::string get_element_name() noexcept override { return element_name; }

	NVGcolor background_color{ 0.1,0.1,0.1,1 };
	NVGcolor hover_color{ 0.32,0.32,0.32,1 };
	NVGcolor press_color{ 0.25,0.25,0.25,1 };
	NVGcolor focus_color{ 0.3,0.3,0.3,1 };

	std::function<void()> callback = []{};
	std::function<void()> double_click_callback = []{};

	button();

	void set_style(style::style_t const& style) override;

	void enable();
	void disable();

protected:
	bool is_pressed = false;
};

}
