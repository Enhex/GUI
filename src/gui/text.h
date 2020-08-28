#pragma once

#include "element.h"

struct text : element
{
	inline static constexpr auto element_name{ "text" };
	std::string get_element_name() override { return element_name; }

	std::type_info const& type_info() const override { return typeid(text); }

	std::string str;
	int font = -1;
	float font_size = 12;
	NVGcolor color = nvgRGBA(255, 255, 255, 255);
	float text_bounds[4];


	text()
	{
		style = element_name;
		apply_style();
	}

	void setup(int new_font, float new_font_size, std::string const& new_str)
	{
		setup(context->vg, new_font, new_font_size, new_str);
	}

	void setup(NVGcontext* vg, int new_font, float new_font_size, std::string const& new_str)
	{
		font = new_font;
		font_size = new_font_size;
		str = new_str;

		update_bounds(vg);
	}

	void set_text(std::string const& new_str) {
		str = new_str;
		update_bounds(context->vg);
	}

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

		bool bounds_need_update = false;
		
		std::string font_name;
		if (read(font_name, "font")) {
			font = context->style_manager.font_name_to_id(font_name);
			bounds_need_update = true;
		}

		if(read(font_size, "font_size")) {
			bounds_need_update = true;	
		}

		if(bounds_need_update)
			update_bounds();

		read(color, "color");
	}

	void update_bounds() { update_bounds(context->vg); }
	void update_bounds(NVGcontext* vg)
	{
		nvgSave(vg);

		init_font(vg);

		nvgTextBounds(vg, 0, 0, str.c_str(), nullptr, text_bounds);

		float ascender, descender, lineh;
		nvgTextMetrics(vg, &ascender, &descender, &lineh);

		// using local space with position 0, can directly use max point instead of calculating bounding box size
		X(min_size) = text_bounds[2];
		Y(min_size) = text_bounds[3] + ascender;

		apply_min_size();

		nvgRestore(vg);
	}

	void init_font(NVGcontext* vg)
	{
		nvgFontSize(vg, font_size);
		nvgFontFaceId(vg, font);
		//nvgTextAlign(vg, NVG_ALIGN_LEFT | NVG_ALIGN_BASELINE);
	}

	void draw(NVGcontext* vg) override
	{
		init_font(vg);
		nvgFillColor(vg, color);

		float ascender, descender, lineh;
		nvgTextMetrics(vg, &ascender, &descender, &lineh);
		auto absolute_position = get_position();
		nvgText(vg, X(absolute_position), Y(absolute_position) + ascender, str.c_str(), nullptr);

		update_bounds(vg);
	}
};