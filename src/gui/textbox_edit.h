#pragma once

#include "textbox.h"
#include "text_edit_shared.h"

// editable text
struct textbox_edit : textbox, text_edit_shared
{
	inline static constexpr auto element_name{ "textbox_edit" };
	std::string get_element_name() override { return element_name; }

	std::type_info const& type_info() const override { return typeid(textbox_edit); }

	textbox_edit();

	size_t cursor_row = 0; // the row the cursor position is inside

	// what to do when the string changes.
	// default is to update the glyphs and call a callback.
	virtual void on_str_changed();

	void update_glyph_positions();

	void set_cursor_to_mouse_pos();
	void on_mouse_press();

	void on_frame_start();

	void on_key_press(int key, int mods);
	void on_character(unsigned codepoint);

	void draw_selection_background(NVGcontext* vg, float const lineh);
	void draw(NVGcontext* vg) override;

	void update_text() override;

	// absolute position may change after layout
	//TODO would be better to only update when position changes
	void post_layout() override;

	void delete_selection();
	void select_all();

	void move_cursor_to_end();
	void move_cursor_to_start();

	void set_cursor_pos(size_t pos);
	size_t get_cursor_row();
};