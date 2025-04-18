#include "text_edit_shared.h"

bool text_edit_shared::has_selection() const
{
	return selection_start_pos != selection_end_pos;
}

void text_edit_shared::clear_selection()
{
	selection_start_pos = selection_end_pos = 0;
}

int text_edit_shared::shared_select_code(int mods)
{
	auto const select = mods & GLFW_MOD_SHIFT;
	if(select && !has_selection()) {
		selection_start_pos = cursor_pos;
	}
	if(!select && has_selection()) {
		clear_selection();
	}
	return select;
}

bool text_edit_shared::is_valid_character(unsigned const codepoint) const
{
	if(valid_characters == nullptr)
		return true;

	for(auto const& c : *valid_characters) {
		if(codepoint == c)
			return true;
	}

	return false;
}

void text_edit_shared::set_cursor_pos(size_t const pos, bool unique)
{
	if(unique && pos == cursor_pos){
		return;
	}
	const_cast<size_t&>(last_cursor_pos) = cursor_pos;
	const_cast<size_t&>(cursor_pos) = pos;
}

void text_edit_shared::on_str_changed(size_t str_size)
{
	if(cursor_pos > str_size){
		const_cast<size_t&>(cursor_pos) = str_size;
		const_cast<size_t&>(last_cursor_pos) = cursor_pos;
	}
}
