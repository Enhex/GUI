#pragma once

#include "button.h"

namespace gui
{

struct checkbox : button
{
	inline static constexpr auto element_name{ "checkbox" };
	std::string get_element_name() noexcept override { return element_name; }

	std::type_info const& type_info() const noexcept override { return typeid(checkbox); }

	NVGcolor inner_color{ 0.08,0.08,0.08,1 };
	NVGcolor selected_color{ 0.3,0.3,0.3,1 };
	NVGcolor unselected_color{ 0.22,0.22,0.22,1 };

	panel* fill;

	checkbox();

	void set_state(bool new_state);
	bool get_state() const;
	void toggle();

protected:
	bool state = false;
};

}
