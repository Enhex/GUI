#include "text_edit_shared.h"

namespace gui
{

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

std::optional<float> text_edit_shared::get_char_pos_x(size_t char_pos, nx::Vector2 const& absolute_position, spans_t const& spans, float const tab_width)
{
	if(char_pos == 0){
		return absolute_position.x; // may have no characters, position at the start.
	}
	else{
		size_t pos = 0;
		for(auto const& span : spans){
			// tabs
			pos += span.leading_tabs;
			if(char_pos <= pos){
				auto const tabs = pos - char_pos;
				return span.offset - (tabs * tab_width) + absolute_position.x;
			}
			// glyphs
			auto const start_pos = pos;
			pos += span.size();
			if(char_pos <= pos){
				auto const glyph_index = (char_pos-1) - start_pos;
				return span.glyphs[glyph_index].maxx;
			}
		}
	}
	// char_pos is outside the spans
	return {};
}

text_edit_shared::glyph_click_result_t text_edit_shared::check_clicked(float mouse_x, float x_min, float x_max, float x_mid) const
{
	// check if the glyph was clicked on its left side
	if(mouse_x >= x_min &&
	   mouse_x <= x_mid)
	{
		return glyph_click_result_t::left;
	}
	// or right side
	else if(mouse_x >= x_mid &&
			mouse_x <= x_max)
	{
		return glyph_click_result_t::right;
	}
	return glyph_click_result_t::not_clicked;
}

void text_edit_shared::on_str_changed(size_t str_size)
{
	if(cursor_pos > str_size){
		const_cast<size_t&>(cursor_pos) = str_size;
		const_cast<size_t&>(last_cursor_pos) = cursor_pos;
	}
}

}
