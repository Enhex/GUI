#pragma once

// for code reuse between text_edit and textbox_edit
//NOTE: should be inherited after text
struct text_edit_shared
{
	//NOTE: using pointer because it allows reusing character sets and takes less space than u32string
	std::u32string const* valid_characters = nullptr;

	NVGcolor selection_color{ 0,0.5,1,1 };

	size_t cursor_pos = 0;
	size_t selection_start_pos = 0;
	size_t selection_end_pos = 0;

	// callback for when the text is changed
	std::function<void()> on_text_changed = []{};

	bool has_selection() const;
	void clear_selection();

	bool is_valid_character(unsigned const codepoint) const;
};