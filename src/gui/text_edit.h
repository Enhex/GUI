#pragma once

#include "text.h"

// editable text
/*TODO
- draw selection rectangle before drawing text
- draw cursor(caret) after drawing text
*/
struct text_edit : text
{
	inline static constexpr auto element_name{ "text_edit" };
	std::string get_element_name() override { return element_name; }

	std::type_info const& type_info() const override { return typeid(text_edit); }

	text_edit();

	size_t cursor_pos = 0;

	// glyphs' absolute positions
	std::unique_ptr<NVGglyphPosition[]> glyphs;
	int num_glyphs = 0;

	void update_glyphs();

	void on_mouse_press();

	void on_key_press(int key, int mods);

	void on_character(unsigned codepoint);

	void draw(NVGcontext* vg) override;
};