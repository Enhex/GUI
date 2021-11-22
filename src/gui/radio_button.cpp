#include "radio_button.h"

#include "border.h"

radio_button::radio_button()
{
	style = element_name;

	color = background_color = unselected_color;
	hover_color = selected_color;

	callback = [this]{
		select();
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

void radio_button::select()
{
	// deselect the rest of the group
	for(auto* btn : groups.at(group)) {
		btn->deselect();
	}

	// visuals
	color = background_color = selected_color;
	fill->color = selected_color;

	if(on_select)
		on_select();
}

void radio_button::deselect()
{
	color = background_color = unselected_color;
	fill->color = inner_color;
}

void radio_button::set_group(std::string const& new_group)
{
	// do nothing if it's the same group
	if(group == new_group)
		return;

	// remove from old group
	auto old_group_iter = groups.find(group);
	if(old_group_iter != groups.end()) {
		auto& buttons = old_group_iter->second;
		auto iter = std::find(buttons.begin(), buttons.end(), this);
		if(iter != buttons.end()) {
			buttons.erase(iter);
		}
	}

	// add to new group
	group = new_group;
	groups[group].emplace_back(this);
}