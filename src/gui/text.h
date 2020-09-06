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


	text();

	void setup(int new_font, float new_font_size, std::string const& new_str);

	void setup(NVGcontext* vg, int new_font, float new_font_size, std::string const& new_str);

	virtual void set_text(std::string const& new_str);

	void set_style(style::style_t const& style) override;

	void update_bounds();
	void update_bounds(NVGcontext* vg);

	void init_font(NVGcontext* vg);

	void draw(NVGcontext* vg) override;
};