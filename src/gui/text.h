#pragma once

#include "element.h"

#include "text_span.h"

struct text : element
{
	inline static constexpr auto element_name{ "text" };
	std::string get_element_name() noexcept override { return element_name; }

	std::type_info const& type_info() const noexcept override { return typeid(text); }

	std::string str;
	int font = -1;
	float font_size = 12;
	NVGcolor color = nvgRGBA(255, 255, 255, 255);

	std::vector<text_span_t> spans;
	float tab_width = 0;

	text();

	void setup(int new_font, float new_font_size, std::string const& new_str);

	void setup(NVGcontext* vg, int new_font, float new_font_size, std::string const& new_str);

	void set_text(std::string const& new_str);
	virtual void update_text();

	void set_style(style::style_t const& style) override;

	void update_spans();

	void update_spans_and_size();

	// both size and min_size
	void update_size();

	void init_font(NVGcontext* vg);

	void draw(NVGcontext* vg) override;
};