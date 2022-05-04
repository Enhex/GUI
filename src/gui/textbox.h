#pragma once

#include "text.h"

/* a multi-line text.
unlike single line text that updates its size based on the text, here the text may auto-split
across lines so the size isn't derived from the text.
*/
struct textbox : text
{
	inline static constexpr auto element_name{ "textbox" };
	std::string get_element_name() override { return element_name; }

	std::type_info const& type_info() const override { return typeid(textbox); }

	// should the textbox fit its size to the string or should it have fixed size and split the string?
	bool fixed_size = false;


	textbox();

	void update_bounds();
	void update_bounds(NVGcontext* vg);
	void draw(NVGcontext* vg) override;
};