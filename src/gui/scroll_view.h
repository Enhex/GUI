#pragma once

#include "element.h"

struct scissor;
struct scrollbar;

/*TODO
- hold middle mouse click to scroll in all directions
- virtual create_child(), to add to the content scissor
*/
struct scroll_view : element
{
	inline static constexpr auto element_name{ "scroll_view" };
	std::string get_element_name() override { return element_name; }

	std::type_info const& type_info() const override { return typeid(scroll_view); }

	element& vertical_container;
	scissor& view;
	element& content; // used to move all the elements inside the view
	std::array<scrollbar*, layout::orientation::count> scroll;

	float scroll_step = 50;

	// position-local to the handle, which mouse drag will be aligned with.
	float handle_drag_ancor;

	scroll_view();

	void subscribe_scroll_events(layout::orientation const orient);

	void stop_dragging(layout::orientation const orient, int const button);

	float get_scroll_length(layout::orientation const orient) const;

	void move_content(layout::orientation const orient, float change);

	void update_handle_size(layout::orientation const orient);

	void update_handle_position(layout::orientation const orient);

	void post_layout() override;
};