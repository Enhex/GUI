#pragma once

#include "text_span.h"

// for code reuse between text_edit and textbox_edit
//NOTE: should be inherited after text
struct text_edit_shared
{
	//NOTE: using pointer because it allows reusing character sets and takes less space than u32string
	std::u32string const* valid_characters = nullptr;

	NVGcolor selection_color{ 0,0.5,1,1 };

	size_t const cursor_pos = 0;
	size_t const last_cursor_pos = 0;
	size_t selection_start_pos = 0;
	size_t selection_end_pos = 0;

	// callback for when the text is changed
	std::function<void()> on_text_changed = []{};

	bool has_selection() const;
	void clear_selection();
	int shared_select_code(int mods);

	bool is_valid_character(unsigned const codepoint) const;

	// if unique is true will only update last_cursor_pos if pos != cursor_pos
	void set_cursor_pos(size_t const pos, bool unique=false);

	// for a single line represented by spans
	// 'char_pos' is relative to the first span's start
	// if char_pos is not in the spans return empty optional
	std::optional<float> get_char_pos_x(size_t char_pos, nx::Vector2 const& absolute_position, spans_t const& spans, float const tab_width);

	enum glyph_click_result_t : uint_least8_t{
		not_clicked,
		left,
		right
	};

	glyph_click_result_t check_clicked(float mouse_x, float x_min, float x_max, float x_mid) const;

	void on_str_changed(size_t str_size);
};
