#pragma once

#include "element.h"

#include "text_span.h"

struct text_base : element
{
	std::string str;
	int font = -1;
	float font_size = 12;
	NVGcolor color = nvgRGBA(255, 255, 255, 255);

	float tab_width = 0;

	text_base();

	void setup(int new_font, float new_font_size, std::string const& new_str);

	void setup(NVGcontext* vg, int new_font, float new_font_size, std::string const& new_str);

	void set_text(std::string const& new_str);
	virtual void update_text() = 0;

	void set_style(style::style_t const& style) override;

	void update_spans(std::string_view const str, spans_t& spans, nx::Vector2 const& abs_pos);

	void init_font(NVGcontext* vg);
};