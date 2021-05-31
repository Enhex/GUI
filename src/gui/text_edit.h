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

	NVGcolor selection_color{ 0,0.5,1,1 };

	size_t cursor_pos = 0;
	size_t selection_start_pos = 0;
	size_t selection_end_pos = 0;

	// glyphs' absolute positions
	std::unique_ptr<NVGglyphPosition[]> glyphs;
	int num_glyphs = 0;

	// callback for when the text is changed
	std::function<void()> on_text_changed = []{};

	// what to do when the string changes.
	// default is to update the glyphs and call a callback.
	virtual void on_str_changed();

	void update_glyph_positions();

	void update_glyphs_no_bounds();

	void update_glyphs();

	void set_cursor_to_mouse_pos();

	void on_mouse_press();

	void on_frame_start();

	void on_key_press(int key, int mods);

	void on_character(unsigned codepoint);

	void draw(NVGcontext* vg) override;

	void set_text(std::string const& new_str) override;

	// absolute position may change after layout
	//TODO would be better to only update when position changes
	void post_layout() override;

	bool has_selection() const;
	void clear_selection();
	void delete_selection();
	void select_all();
};