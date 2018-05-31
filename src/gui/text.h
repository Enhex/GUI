#pragma once

#include "element.h"

struct text : element
{
	std::string str;
	int font = -1;
	float font_size = 12;
	NVGcolor color = nvgRGBA(255, 255, 255, 255);


	text()
	{
		auto iter = context->style_manager.styles.find("text");
		if (iter != context->style_manager.styles.end()) {
			set_style(iter->second);
		}
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

	void set_style(style::style_t const& style)
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

		if (read(font, "font") | read(font_size, "font_size")) {
			update_bounds();
		}
		read(color, "color");
	}

	void update_bounds() { update_bounds(context->vg); }
	void update_bounds(NVGcontext* vg)
	{
		nvgSave(vg);

		init_font(vg);

		float bounds[4];
		nvgTextBounds(vg, 0, 0, str.c_str(), nullptr, bounds);

		float ascender, descender, lineh;
		nvgTextMetrics(vg, &ascender, &descender, &lineh);

		// using local space with position 0, can directly use max point instead of calculating bounding box size
		X(size) = X(min_size) = bounds[2];
		Y(size) = Y(min_size) = bounds[3] + ascender;


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
		nvgText(vg, X(position), Y(position) + ascender, str.c_str(), nullptr);

		update_bounds(vg);
	}
};