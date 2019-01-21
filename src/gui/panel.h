#pragma once

#include "element.h"

struct panel : element
{
	inline static constexpr auto element_name{ "panel" };
	std::string get_element_name() override { return element_name; }

	std::type_info const& type_info() const override { return typeid(panel); }

	panel()
	{
		style = element_name;
	}

	NVGcolor color{0,0,0,1};


	void set_style(style::style_t const& style) override
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

	void draw(NVGcontext* vg) override
	{
		nvgBeginPath(vg);
		auto absolute_position = get_position();
		nvgRect(vg, X(absolute_position), Y(absolute_position), X(size), Y(size));
		nvgFillColor(vg, color);
		nvgFill(vg);
	};
};