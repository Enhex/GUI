#pragma once

#include "text.h"
#include "text_edit_shared.h"

// editable text
struct text_edit : text, text_edit_shared
{
	inline static constexpr auto element_name{ "text_edit" };
	std::string get_element_name() override { return element_name; }

	std::type_info const& type_info() const override { return typeid(text_edit); }

	// glyphs' absolute positions
	std::unique_ptr<NVGglyphPosition[]> glyphs;
	int num_glyphs = 0;

	text_edit();

	// what to do when the string changes.
	// default is to update the glyphs and call a callback.
	virtual void on_str_changed();

	void update_glyph_positions();

	void update_glyphs_no_bounds();

	void update_glyphs();

	void set_cursor_to_mouse_pos();

	void on_mouse_press();

	void on_frame_start();

	void on_double_click();

	void on_key_press(int key, int mods);

	void on_character(unsigned codepoint);

	void draw(NVGcontext* vg) override;

	void update_text() override;

	// absolute position may change after layout
	//TODO would be better to only update when position changes
	void post_layout() override;

	void delete_selection();
	void select_all();

	void move_cursor_to_end();
};