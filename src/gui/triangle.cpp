#include "triangle.h"

triangle::triangle()
{
	style = element_name;
}

void triangle::set_style(style::style_t const& style)
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

	read(color, "color");
}

void triangle::draw(NVGcontext* vg)
{
	nvgBeginPath(vg);
	auto abs_pos = get_position();
	nvgMoveTo(vg, abs_pos.x + vertices[0].x, abs_pos.y + vertices[0].y);
	nvgLineTo(vg, abs_pos.x + vertices[1].x, abs_pos.y + vertices[1].y);
	nvgLineTo(vg, abs_pos.x + vertices[2].x, abs_pos.y + vertices[2].y);
	nvgFillColor(vg, color);
	nvgFill(vg);
}