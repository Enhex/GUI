#include "checkbox.h"

#include "border.h"

checkbox::checkbox()
{
	style = element_name;
	
	color = background_color = unselected_color;
	hover_color = selected_color;

	callback = [this]{
		toggle();
	};
	
	min_size = {20,20};

	auto& outer_border = create_child<border>();
	outer_border.set_color({0,0,0,0});
	outer_border.set_border_size(2);
	outer_border.expand = {1,1};
	outer_border.content.create_layout<gui::layout::box>();
	outer_border.content.expand = {1,1};

	auto& border_ = outer_border.content.create_child<border>();
	border_.set_color(inner_color);
	border_.set_border_size(2);
	border_.expand = {1,1};
	border_.content.create_layout<gui::layout::box>();
	border_.content.expand = {1,1};

	fill = &border_.content.create_child<panel>();
	fill->color = inner_color;
	fill->expand = {1,1};
}

void checkbox::set_state(bool new_state)
{
	state = new_state;
	if(state) {
		color = background_color = selected_color;
		fill->color = selected_color;
	}
	else {
		color = background_color = unselected_color;
		fill->color = inner_color;
	}
}

bool checkbox::get_state() const
{
	return state;
}

void checkbox::toggle()
{
	set_state(!state);
}