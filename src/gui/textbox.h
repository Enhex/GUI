#pragma once

#include "element.h"

/* a multi-line text.
unlike single line text that updates its size based on the text, here the text may auto-split
across lines so the size isn't derived from the text.
*/
struct textbox : element
{
	inline static constexpr auto element_name{ "textbox" };
	std::string get_element_name() override { return element_name; }

	std::type_info const& type_info() const override { return typeid(textbox); }

	std::string str;
	int font = -1;
	float font_size = 12;
	NVGcolor color = nvgRGBA(255, 255, 255, 255);
	float text_bounds[4]; // local space
	// should the textbox fit its size to the string or should it have fixed size and split the string?
	bool fixed_size = false;
	std::vector<NVGtextRow> rows;


	textbox();

	void setup(int new_font, float new_font_size, std::string const& new_str);

	void setup(NVGcontext* vg, int new_font, float new_font_size, std::string const& new_str);

	virtual void set_text(std::string const& new_str);

	void set_style(style::style_t const& style) override;

	void update_bounds();
	void update_bounds(NVGcontext* vg);

	void init_font(NVGcontext* vg);

	void draw(NVGcontext* vg) override;
};