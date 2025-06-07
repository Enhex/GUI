#pragma once

#include "text.h"
#include "text_edit_shared.h"

namespace gui
{

// editable text
struct text_edit : text, text_edit_shared
{
	inline static constexpr auto element_name{ "text_edit" };
	std::string get_element_name() noexcept override { return element_name; }

	std::type_info const& type_info() const noexcept override { return typeid(text_edit); }

	text_edit();

	// what to do when the string changes.
	// default is to update the glyphs and call a callback.
	virtual void on_str_changed();
	void on_str_changed_no_callback();

	void update_glyph_positions();

	void set_cursor_to_mouse_pos();

	void on_mouse_press();

	void on_frame_start();

	void on_double_click();

	void on_key_press(int key, int mods);

	void on_character(unsigned codepoint);

	float get_char_pos_x(size_t char_pos, nx::Vector2 const& absolute_position);

	void draw(NVGcontext* vg) override;

	void update_text() override;

	// absolute position may change after layout
	//TODO would be better to only update when position changes
	void post_layout() override;

	void delete_selection();
	void select_all();

	void move_cursor_to_end();
};

}
