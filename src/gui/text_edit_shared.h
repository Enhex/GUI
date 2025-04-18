#pragma once

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

	void on_str_changed(size_t str_size);
};
