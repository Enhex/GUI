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

void textbox_edit::update_rows()
{
	if(!fixed_size)
		update_bounds();

	init_font(context->vg);
	float ascender;
	nvgTextMetrics(context->vg, &ascender, nullptr, nullptr);
	auto absolute_position = get_position();
	rows = nvgTextBoxGetRows(context->vg, X(absolute_position), Y(absolute_position) + ascender, X(size), str.c_str(), nullptr);
}

void textbox_edit::on_str_changed()
{
	update_rows();
	update_glyphs();
	on_text_changed();
}

void textbox_edit::update_glyph_positions()
{
	auto& vg = context->vg;
	nvgSave(vg);
	init_font(vg); // for correct font size

	auto const max_glyphs = str.size();
	auto const absolute_position = get_position();
	num_glyphs = nvgTextGlyphPositions(vg, X(absolute_position), Y(absolute_position), str.c_str(), nullptr, glyphs.get(), (int)max_glyphs);

	// nvgTextGlyphPositions only works with single line so characters in a new line got wrong X position as if they're all in a single line,
	// so fix the positions.
	//NOTE: can't edit positions directly otherwise nanovg crashes.
	//NOTE: starting from i=1 because first line doesn't need offsetting, except the newline of the first line.
	glyph_offsets.resize(max_glyphs);
	// offset the first line's newline
	if(rows.size() > 1) { // at least 1 line and it isn't the last one
		auto const glyph_x_offset = glyphs[rows[0].start - str.data()].minx - X(absolute_position);
		glyph_offsets[rows[0].end - str.data()] = glyph_x_offset;
	}
	// offset the rest of the lines
	for (size_t i=0; i < rows.size(); ++i)
	{
		auto const& row = rows[i];
		auto const glyph_x_offset = glyphs[row.start - str.data()].minx - X(absolute_position);
		for(auto p = row.start; p < row.end; ++p) {
			glyph_offsets[p - str.data()] = glyph_x_offset;
		}
		// newline also gets a glyph, excluding the last row since it doesn't have newline.
		//last row actually doesn't have a newline?
		if(i < rows.size()-1) {
			glyph_offsets[row.end - str.data()] = glyph_x_offset;
		}
	}

	if(cursor_pos > num_glyphs)
		set_cursor_pos(num_glyphs);

	nvgRestore(vg);
}

void textbox_edit::update_glyphs_no_bounds()
{
	auto const max_glyphs = str.size();

	glyphs = std::make_unique<NVGglyphPosition[]>(max_glyphs);

	update_glyph_positions();
}

void textbox_edit::update_glyphs()
{
	update_glyphs_no_bounds();
	update_bounds();
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
		if (mouse_y >= row_y &&
			mouse_y <= row_y + lineh)
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
		if (cursor_pos > 0)
		{
			auto const select = mods & GLFW_MOD_SHIFT;
			if(select && !has_selection()) {
				selection_start_pos = cursor_pos;
			}

			set_cursor_pos(cursor_pos-1);

			if(select)
				selection_end_pos = cursor_pos;
		}
		break;

	case GLFW_KEY_RIGHT:
		if (cursor_pos < str.size())
		{
			auto const select = mods & GLFW_MOD_SHIFT;
			if(select && !has_selection()) {
				selection_start_pos = cursor_pos;
			}

			set_cursor_pos(cursor_pos+1);

			if(select)
				selection_end_pos = cursor_pos;
		}
		break;

	case GLFW_KEY_DOWN:
		// move a line down
		if (!rows.empty() && cursor_row < rows.size()-1)
		{
			auto const select = mods & GLFW_MOD_SHIFT;
			if(select && !has_selection()) {
				selection_start_pos = cursor_pos;
			}

			auto const pos_in_row = cursor_pos- (rows[cursor_row].start - str.data());
			auto const& next_row = rows[cursor_row+1];
			auto const next_row_size = next_row.end - next_row.start;
			if(pos_in_row <= next_row_size) {
				auto const next_row_start_pos = next_row.start - str.data();
				cursor_pos = next_row_start_pos + pos_in_row;
				++cursor_row;
			}
			else {
				auto const next_row_end_pos = next_row.end-1 - str.data();
				cursor_pos = next_row_end_pos;
				++cursor_row;
			}

			if(select)
				selection_end_pos = cursor_pos;
		}
		else {
			move_cursor_to_end();
		}
		break;

	case GLFW_KEY_UP:
		// move a line up
		if (cursor_row > 0)
		{
			auto const select = mods & GLFW_MOD_SHIFT;
			if(select && !has_selection()) {
				selection_start_pos = cursor_pos;
			}

			auto const pos_in_row = cursor_pos - (rows[cursor_row].start - str.data());
			auto const& prev_row = rows[cursor_row-1];
			auto const prev_row_size = prev_row.end - prev_row.start;
			if(pos_in_row <= prev_row_size) {
				auto const prev_row_start_pos = prev_row.start - str.data();
				cursor_pos = prev_row_start_pos + pos_in_row;
				--cursor_row;
			}
			else {
				auto const prev_row_end_pos = prev_row.end-1 - str.data();
				cursor_pos = prev_row_end_pos;
				--cursor_row;
			}

			if(select)
				selection_end_pos = cursor_pos;
		}
		else {
			move_cursor_to_start();
		}
		break;

	case GLFW_KEY_HOME:
		set_cursor_pos(0);
		break;

	case GLFW_KEY_END:
		set_cursor_pos(str.size());
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
		set_cursor_pos(cursor_pos+1);
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

void textbox_edit::draw(NVGcontext* vg)
{
	float ascender, descender, lineh;
	init_font(vg);
	nvgTextMetrics(vg, &ascender, &descender, &lineh);

	// draw selection background
	if(has_selection())
	{
		if (glyphs == nullptr)
			update_glyphs();

		auto const absolute_position = get_position();
		// sort positions
		auto const start_pos = selection_start_pos > selection_end_pos ? selection_end_pos : selection_start_pos;
		auto const end_pos = selection_start_pos > selection_end_pos ? selection_start_pos : selection_end_pos;

		bool selection_started = false;
		bool selection_ended = false;

		// draw rectangle for each line
		for(size_t i=0; i < rows.size(); ++i) {
			auto const& row = rows[i];
			auto const start_ptr = str.data() + start_pos;
			auto const end_ptr = str.data() + end_pos;

			// skip rows before selection
			auto const is_start_in_row = start_ptr >= row.start && start_ptr < row.end;
			if(!is_start_in_row && !selection_started)
				continue;

			auto const y_start = Y(absolute_position) + i * lineh;

			auto const x_start = [&]{
				// if selection already started highlight from the start of the row
				if(!selection_started && is_start_in_row && start_pos > 0) {
					auto const index = start_pos-1;
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
				return X(absolute_position) + row.maxx;
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
				auto const index = cursor_pos-1;
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
	str = new_str;
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