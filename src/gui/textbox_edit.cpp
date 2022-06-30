#include "textbox_edit.h"

#include "../framework/application.h"
#include "../include_glfw.h"
#include "nvgTextBoxRows.h"

textbox_edit::textbox_edit()
{
	style = element_name;

	auto& input_manager = context->input_manager;

	input_manager.mouse_press.subscribe(this, [this](int button, int mods) {
		on_mouse_press();
	});
	input_manager.mouse_release.subscribe_global(this, [this](int button, int mods) {
		context->input_manager.frame_start.unsubscribe_global(this);
	});
	input_manager.key_press.subscribe(this, [this](int key, int mods) {
		on_key_press(key, mods);
	});
	input_manager.key_repeat.subscribe(this, [this](int key, int mods) {
		on_key_press(key, mods);
	});
	input_manager.character.subscribe(this, [this](unsigned int codepoint) {
		on_character(codepoint);
	});

	input_manager.focus_end.subscribe(this, [this]() {
		clear_selection();
	});

	// subscribe so textbox_edit will be able to capture focus
	input_manager.focus_start.subscribe(this, [this]() {});
}

void textbox_edit::on_str_changed()
{
	update_rows();
	update_glyphs();
	on_text_changed();
}

void textbox_edit::update_glyph_positions()
{
	textbox::update_glyph_positions();
	if(cursor_pos > num_glyphs)
		set_cursor_pos(num_glyphs);
}

void textbox_edit::set_cursor_to_mouse_pos()
{
	auto& input_manager = context->input_manager;
	auto const mouse_x = X(input_manager.mouse_pos);
	auto const mouse_y = Y(input_manager.mouse_pos);

	auto const abs_pos = get_position();

	float ascender, descender, lineh;
	init_font(context->vg);
	nvgTextMetrics(context->vg, &ascender, &descender, &lineh);

	/*
	for each row check if the cursor is inside its Y interval.
	if it is check if it's inside one of its glyphs' X interval.
	if it isn't place the cursor at the end of the line.
		- need to check row X interval?
			- if clicked outside the element it shouldn't have focus thus no cursor.
	*/
	for (size_t i=0; i < rows.size(); ++i)
	{
		auto const& row = rows[i];
		auto const row_y = Y(abs_pos) + (i * lineh);

		// check if inside Y
		// exluding upper bound check for first row and lower bound check for last row
		bool is_in_row;
		if(i == 0) {
			is_in_row = mouse_y <= row_y + lineh;
			// if before first row move to start
			if(is_in_row && mouse_y < row_y) {
				move_cursor_to_start();
				return;
			}
		}
		else if(i == rows.size()-1) {
			is_in_row = mouse_y >= row_y;
			// if after last row move to end
			if(is_in_row && mouse_y > row_y + lineh) {
				move_cursor_to_end();
				return;
			}
		}
		else {
			is_in_row = mouse_y >= row_y &&
						mouse_y <= row_y + lineh;
		}

		if (is_in_row)
		{
			// check if inside a glyph
			auto const row_end = row.end - str.data();
			for(auto glyph_index = row.start - str.data(); glyph_index != row_end; ++glyph_index)
			{
				auto const& glyph = glyphs[glyph_index];
				auto const& x_offset = glyph_offsets[glyph_index];
				auto const glyph_x = glyph.x - x_offset;
				auto const minx = glyph.minx - x_offset;
				auto const maxx = glyph.maxx - x_offset;

				// clicking on the left side of a glyph positions the cursor before it, and right side after it.
				auto const x_mid = glyph_x + (maxx - minx) / 2;

				// check if the glyph was clicked on its left side
				if (mouse_x >= minx &&
					mouse_x <= x_mid)
				{
					set_cursor_pos(glyph_index);
					goto glyph_found;
				}
				// or right side
				else if(mouse_x >= x_mid &&
						mouse_x <= maxx)
				{
					set_cursor_pos(glyph_index+1);
					goto glyph_found;
				}
			}

			// place at the end of the row
			set_cursor_pos(row_end);
			glyph_found:
			// if inside this row can't be inside other rows
			break;
		}
	}
}

void textbox_edit::on_mouse_press()
{
	clear_selection();
	set_cursor_to_mouse_pos();

	selection_start_pos = cursor_pos;

	context->input_manager.frame_start.subscribe_global(this, [this]() {
		on_frame_start();
	});
}

void textbox_edit::on_frame_start()
{
	set_cursor_to_mouse_pos();
	selection_end_pos = cursor_pos;
}

void textbox_edit::on_key_press(int key, int mods)
{
	auto shared_select_code = [&]{
		auto const select = mods & GLFW_MOD_SHIFT;
		if(select && !has_selection()) {
			selection_start_pos = cursor_pos;
		}
		if(!select && has_selection()) {
			clear_selection();
		}
		return select;
	};

	switch (key) {
	case GLFW_KEY_BACKSPACE:
		if(has_selection()) {
			delete_selection();
		}
		else if (cursor_pos > 0) {
			set_cursor_pos(cursor_pos-1);
			str.erase(cursor_pos, 1);
			on_str_changed();
		}
		break;

	case GLFW_KEY_DELETE:
		if(has_selection()) {
			delete_selection();
		}
		else if (cursor_pos < str.size()) {
			str.erase(cursor_pos, 1);
			on_str_changed();
		}
		break;

	case GLFW_KEY_LEFT:
	{
		auto const select = shared_select_code();

		if (cursor_pos > 0)
		{
			set_cursor_pos(cursor_pos-1);

			if(select)
				selection_end_pos = cursor_pos;
		}
		break;
	}
	case GLFW_KEY_RIGHT:
	{
		auto const select = shared_select_code();

		if (cursor_pos < str.size())
		{
			set_cursor_pos(cursor_pos+1);

			if(select)
				selection_end_pos = cursor_pos;
		}
		break;
	}
	case GLFW_KEY_DOWN:
	{
		// move a line down
		auto const select = shared_select_code();

		if (!rows.empty() && cursor_row < rows.size()-1)
		{
			auto const pos_in_row = cursor_pos- (rows[cursor_row].start - str.data());
			auto const& next_row = rows[cursor_row+1];
			auto const next_row_size = next_row.end - next_row.start;
			if(pos_in_row <= next_row_size) {
				auto const next_row_start_pos = next_row.start - str.data();
				cursor_pos = next_row_start_pos + pos_in_row;
			}
			else {
				auto const next_row_end_pos = next_row.end - str.data();
				cursor_pos = next_row_end_pos;
			}
			++cursor_row;
		}
		else {
			move_cursor_to_end();
		}

		if(select)
			selection_end_pos = cursor_pos;

		break;
	}
	case GLFW_KEY_UP:
	{
		// move a line up
		auto const select = shared_select_code();

		if (cursor_row > 0)
		{
			auto const pos_in_row = cursor_pos - (rows[cursor_row].start - str.data());
			auto const& prev_row = rows[cursor_row-1];
			auto const prev_row_size = prev_row.end - prev_row.start;
			if(pos_in_row <= prev_row_size) {
				auto const prev_row_start_pos = prev_row.start - str.data();
				cursor_pos = prev_row_start_pos + pos_in_row;
			}
			else {
				auto const prev_row_end_pos = prev_row.end - str.data();
				cursor_pos = prev_row_end_pos;
			}
			--cursor_row;
		}
		else {
			move_cursor_to_start();
		}

		if(select)
			selection_end_pos = cursor_pos;

		break;
	}
	case GLFW_KEY_HOME:
		if(!rows.empty()) {
			auto const row_start_pos = rows[cursor_row].start - str.data();
			set_cursor_pos(row_start_pos);
		}
		break;

	case GLFW_KEY_END:
		if(!rows.empty()) {
			auto const row_end_pos = rows[cursor_row].end - str.data();
			set_cursor_pos(row_end_pos);
		}
		break;

	case GLFW_KEY_C:
		if (mods & GLFW_MOD_CONTROL) {
			auto& app = static_cast<application&>(*context);
			auto low_pos = std::min(selection_start_pos, selection_end_pos);
			auto high_pos = std::max(selection_start_pos, selection_end_pos);
			glfwSetClipboardString(app.window, str.substr(low_pos, high_pos - low_pos).c_str());
		}
		break;

	case GLFW_KEY_V:
		if (mods & GLFW_MOD_CONTROL) {
			delete_selection();
			auto& app = static_cast<application&>(*context);
			auto const cstr = glfwGetClipboardString(app.window);
			str.insert(cursor_pos, cstr);
			cursor_pos += strlen(cstr);
			on_str_changed();
		}
		break;

	case GLFW_KEY_A:
		if(mods & GLFW_MOD_CONTROL) {
			select_all();
		}
		break;

	case GLFW_KEY_ENTER:
		on_character('\n');
		++cursor_row;
		break;
	}
}

void textbox_edit::on_character(unsigned codepoint)
{
	if(has_selection()) {
		delete_selection();
	}
	str.insert(str.begin() + cursor_pos++, codepoint);
	on_str_changed();
	//NOTE: no need to update glyphs when deleting since the deleted glyphs won't be accessed.
}

void textbox_edit::draw_selection_background(NVGcontext* vg, float const lineh)
{
	if(!has_selection())
		return;

	if (glyphs == nullptr)
		update_glyphs();

	// sort positions
	auto start_pos = selection_start_pos > selection_end_pos ? selection_end_pos : selection_start_pos;
	auto const end_pos = selection_start_pos > selection_end_pos ? selection_start_pos : selection_end_pos;

	auto const absolute_position = get_position();
	bool selection_started = false;
	bool selection_ended = false;

	// draw rectangle for each line
	for(size_t i=0; i < rows.size(); ++i) {
		auto const& row = rows[i];
		auto const start_ptr = str.data() + start_pos;
		auto const end_ptr = str.data() + end_pos;

		// skip rows before selection
		// include row end in the case of end being a newline
		auto const is_start_in_row = start_ptr >= row.start && start_ptr <= row.end;
		if(!is_start_in_row && !selection_started)
			continue;

		auto const y_start = Y(absolute_position) + i * lineh;

		auto const x_start = [&]{
			// if selection already started highlight from the start of the row
			if(!selection_started && is_start_in_row && start_pos > 0) {
				auto const index = start_pos-1;
				// if the previous character is a newline, use the current character's min
				if(str[index] == '\n')
					return glyphs[start_pos].minx - glyph_offsets[start_pos]; // position at the end of the previous character
				return glyphs[index].maxx - glyph_offsets[index]; // position at the end of the previous character
			}
			return X(absolute_position);
		}();

		// mark after using it to check if selection already started
		selection_started = true;

		auto const x_end = [&]{
			// if not the last row include the end pointer (should be a newline)
			bool is_in_row;
			if(i != rows.size()-1) {
				is_in_row = end_ptr >= row.start && end_ptr <= row.end;
			}
			else {
				is_in_row = end_ptr >= row.start && end_ptr < row.end;
			}
			if(is_in_row && selection_started)
				selection_ended = true;
			if(is_in_row && end_pos > 0) {
				auto const index = end_pos-1;
				return glyphs[index].maxx - glyph_offsets[index]; // position at the end of the previous character
			}
			auto const row_end_pos = row.end-1 - str.data();
			return glyphs[row_end_pos].maxx - glyph_offsets[row_end_pos]; // position at the end of the row
		}();

		nvgBeginPath(vg);
		nvgRect(vg,
				x_start, y_start,
				x_end - x_start, lineh);
		nvgFillColor(vg, selection_color);
		nvgFill(vg);

		// skip rows after selection
		if(selection_ended)
			break;
	}
}

void textbox_edit::draw(NVGcontext* vg)
{
	float ascender, descender, lineh;
	init_font(vg);
	nvgTextMetrics(vg, &ascender, &descender, &lineh);

	draw_selection_background(vg, lineh);

	textbox::draw(vg);

	auto& input_manager = context->input_manager;

	// draw cursor
	if (this == input_manager.focused_element)
	{
		if (glyphs == nullptr)
			update_glyphs();

		auto const absolute_position = get_position();

		// y_pos for line height
		// multiply the number of previous newlines by line height
		auto const y_pos = cursor_row * lineh + Y(absolute_position);

		auto const x_pos = [&]{
			if(cursor_pos == 0)
				return X(absolute_position); // may have no characters, position at the start.
			else if(cursor_pos == num_glyphs) {
				auto index = cursor_pos-1;
				if(str[index] == '\n')
					return X(absolute_position);
				return glyphs[index].maxx - glyph_offsets[index];
			}
			return glyphs[cursor_pos].minx - glyph_offsets[cursor_pos]; // position at the end of the previous character
		}();
		nvgBeginPath(vg);
		nvgMoveTo(vg, x_pos, y_pos);
		nvgLineTo(vg, x_pos, y_pos + lineh);
		nvgStrokeColor(vg, nvgRGBA(255, 255, 255, 255));
		nvgStrokeWidth(vg, 1.0f);
		nvgStroke(vg);
	}
}

void textbox_edit::set_cursor_pos(size_t pos)
{
	cursor_pos = pos;
	cursor_row = get_cursor_row();
}

size_t textbox_edit::get_cursor_row()
{
	size_t y_pos = 0;
	// starting from i=1 because if cursor is in the first row y_pos is 0
	for(size_t i=1; i < rows.size(); ++i) {
		if(cursor_pos < rows[i].start - str.data()) {
			y_pos = i-1;
			break;
		}
	}
	// last line
	if(!rows.empty() && cursor_pos >= rows.back().start - str.data())
		y_pos = rows.size()-1;
	return y_pos;
}

void textbox_edit::set_text(std::string const& new_str)
{
	clear_selection();
	textbox::set_text(new_str);
	on_str_changed();
}


void textbox_edit::post_layout()
{
	// glyph positions may change
	if (glyphs != nullptr)
		update_glyph_positions();
}

void textbox_edit::delete_selection()
{
	//NOTE: if there's no selection 0 chars will be erased
	auto low_pos = std::min(selection_start_pos, selection_end_pos);
	auto high_pos = std::max(selection_start_pos, selection_end_pos);
	str.erase(low_pos, high_pos - low_pos);
	set_cursor_pos(low_pos);
	clear_selection();
	on_str_changed();
}

void textbox_edit::select_all()
{
	selection_start_pos = 0;
	selection_end_pos = str.size();
	set_cursor_pos(selection_end_pos);
}

void textbox_edit::move_cursor_to_end()
{
	cursor_pos = str.size();
	cursor_row = rows.empty() ? 0 : rows.size()-1;
}

void textbox_edit::move_cursor_to_start()
{
	cursor_pos = 0;
	cursor_row = 0;
}