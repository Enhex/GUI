#include "border.h"

#include "panel.h"

namespace gui
{

border::border() :
	top(create_child<panel>()),
	middle(create_child<element>()),
	left(middle.create_child<panel>()),
	content(middle.create_child<element>()),
	right(middle.create_child<panel>()),
	bottom(create_child<panel>())
{
	create_layout<gui::layout::box>();
	middle.create_layout<gui::layout::box>().orient = layout::horizontal;
	middle.expand = {1,1};
	content.create_layout<gui::layout::shrink>();

	top.expand[layout::horizontal] = true;
	bottom.expand[layout::horizontal] = true;
	left.expand[layout::vertical] = true;
	right.expand[layout::vertical] = true;

	set_border_size(1.f);

	style = element_name;
	apply_style();
}

void border::set_color(NVGcolor color)
{
	top.color = bottom.color = left.color = right.color = color;
}

void border::set_border_size(float size)
{
	top.min_size.y = size;
	bottom.min_size.y = size;
	left.min_size.x = size;
	right.min_size.x = size;
}

}
