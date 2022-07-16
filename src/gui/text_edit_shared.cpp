#include "text_edit_shared.h"

bool text_edit_shared::has_selection() const
{
	return selection_start_pos != selection_end_pos;
}

void text_edit_shared::clear_selection()
{
	selection_start_pos = selection_end_pos = 0;
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