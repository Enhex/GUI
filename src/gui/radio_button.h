#pragma once

#include "button.h"

namespace gui
{

// exclusive selection
struct radio_button : button
{
	inline static constexpr auto element_name{ "radio_button" };
	std::string get_element_name() noexcept override { return element_name; }

	std::type_info const& type_info() const noexcept override { return typeid(radio_button); }

	// group -> buttons
	//NOTE: using vector because unlikely to remove elements
	inline static std::unordered_map<std::string, std::vector<radio_button*>> groups;

	NVGcolor inner_color{ 0.08,0.08,0.08,1 };
	NVGcolor selected_color{ 0.3,0.3,0.3,1 };
	NVGcolor unselected_color{ 0.22,0.22,0.22,1 };

	panel* fill;

	std::function<void()> on_select;

	radio_button();

	void select();
	void set_group(std::string const& new_group);

protected:
	std::string group;

	void deselect();
};

}
