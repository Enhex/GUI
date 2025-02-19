#include "panel.h"

panel::panel()
{
	style = element_name;
}

void panel::set_style(style::style_t const& style)
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

void panel::draw(NVGcontext* vg)
{
	nvgBeginPath(vg);
	auto absolute_position = get_position();
	nvgRect(vg, absolute_position.x, absolute_position.y, size.x, size.y);
	nvgFillColor(vg, color);
	nvgFill(vg);
}