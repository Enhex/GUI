#include "text_edit_shared.h"

bool text_edit_shared::has_selection() const
{
	return selection_start_pos != selection_end_pos;
}

void text_edit_shared::clear_selection()
{
	selection_start_pos = selection_end_pos = 0;
}