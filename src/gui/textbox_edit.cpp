#include "textbox_edit.h"

#include "../include_glfw.h"
#include "../framework/application.h"

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
	// starting from i=1 because first line doesn't need offsetting
	for (size_t i=1; i < rows.size(); ++i)
	{
		auto const& row = rows[i];
		auto const glyph_x_offset = glyphs[row.start - str.data()].minx - X(absolute_position);
		for(auto p = row.start; p < row.end; ++p) {
			auto& glyph = glyphs[p - str.data()];
			glyph.x -= glyph_x_offset;
			glyph.minx -= glyph_x_offset;
			glyph.maxx -= glyph_x_offset;
		}
		// newline also gets a glyph
		if(i < rows.size()-1) {
			auto& glyph = glyphs[row.end - str.data()];
			glyph.x -= glyph_x_offset;
			glyph.minx -= glyph_x_offset;
			glyph.maxx -= glyph_x_offset;
		}
	}

	if(cursor_pos > num_glyphs)
		cursor_pos = num_glyphs;

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

				// clicking on the left side of a glyph positions the cursor before it, and right side after it.
				auto const x_mid = glyph.x + (glyph.maxx - glyph.minx) / 2;

				// check if the glyph was clicked on its left side
				if (mouse_x >= glyph.minx &&
					mouse_x <= x_mid)
				{
					cursor_pos = glyph_index;
					goto glyph_found;
				}
				// or right side
				else if(mouse_x >= x_mid &&
						mouse_x <= glyph.maxx)
				{
					cursor_pos = glyph_index+1;
					goto glyph_found;
				}
			}

			// place at the end of the row
			cursor_pos = row_end;
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
			str.erase(--cursor_pos, 1);
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

			--cursor_pos;

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

			++cursor_pos;

			if(select)
				selection_end_pos = cursor_pos;
		}
		break;

	case GLFW_KEY_HOME:
		cursor_pos = 0;
		break;

	case GLFW_KEY_END:
		cursor_pos = str.size();
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

		// draw rectangle for each line
		//TODO

		auto const x_start = selection_start_pos == 0 ?
			X(absolute_position) : // may have no characters, position at the start.
			glyphs[selection_start_pos-1].maxx; // position at the end of the previous character

		auto const x_end = selection_end_pos == 0 ?
			X(absolute_position) :
			glyphs[selection_end_pos-1].maxx;

		nvgBeginPath(vg);
		nvgRect(vg,
				x_start, Y(absolute_position),
				x_end - x_start, lineh);
		nvgFillColor(vg, selection_color);
		nvgFill(vg);
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
		size_t y_pos = 0;
		// starting from i=1 because if cursor is in the first row y_pos is 0
		for(size_t i=1; i < rows.size(); ++i) {
			if(cursor_pos < rows[i].start - str.data()) {
				y_pos = i-1;
				break;
			}
		}
		if(cursor_pos >= rows.back().start - str.data())
			y_pos = rows.size()-1;
		y_pos *= lineh;
		y_pos += Y(absolute_position);

		auto const x_pos = [&]{
			if(cursor_pos == 0)
				return X(absolute_position); // may have no characters, position at the start.
			else if(cursor_pos == num_glyphs)
				return glyphs[cursor_pos-1].maxx;
			return glyphs[cursor_pos].minx; // position at the end of the previous character
		}();
		nvgBeginPath(vg);
		nvgMoveTo(vg, x_pos, y_pos);
		nvgLineTo(vg, x_pos, y_pos + lineh);
		nvgStrokeColor(vg, nvgRGBA(255, 255, 255, 255));
		nvgStrokeWidth(vg, 1.0f);
		nvgStroke(vg);
	}
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

bool textbox_edit::has_selection() const
{
	return selection_start_pos != selection_end_pos;
}

void textbox_edit::clear_selection()
{
	selection_start_pos = selection_end_pos = 0;
}

void textbox_edit::delete_selection()
{
	//NOTE: if there's no selection 0 chars will be erased
	auto low_pos = std::min(selection_start_pos, selection_end_pos);
	auto high_pos = std::max(selection_start_pos, selection_end_pos);
	str.erase(low_pos, high_pos - low_pos);
	cursor_pos = low_pos;
	clear_selection();
	on_str_changed();
}

void textbox_edit::select_all()
{
	selection_start_pos = 0;
	selection_end_pos = str.size();
	cursor_pos = selection_end_pos;
}

void textbox_edit::move_cursor_to_end()
{
	cursor_pos = str.size();
}