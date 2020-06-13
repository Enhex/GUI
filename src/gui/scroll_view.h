#pragma once

#include "element.h"

#include "scrollbar.h"

/*TODO
- set scrollbar handle size relative to size/overflow
- hold middle mouse click to scroll in all directions
- virtual add_child(), to add to the content scissor
*/
struct scroll_view : element
{
	inline static constexpr auto element_name{ "scroll_view" };
	std::string get_element_name() override { return element_name; }

	std::type_info const& type_info() const override { return typeid(scroll_view); }

	scroll_view()
	{
		style = element_name;
		
		auto& box = create_layout<gui::layout::box>();
		box.orient = layout::horizontal;

		auto& content = create_child<scissor>();
		content.expand = {true, true};

		auto& scroll = create_child<scrollbar>();
		scroll.expand = {false, true};
	}
};