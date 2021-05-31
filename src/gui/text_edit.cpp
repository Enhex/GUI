#include "text_edit.h"

#include "../include_glfw.h"
#include "../framework/application.h"

text_edit::text_edit()
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

	// subscribe so text_edit will be able to capture focus
	input_manager.focus_start.subscribe(this, [this]() {});
}

void text_edit::update_glyph_positions()
{
	auto& vg = context->vg;
	nvgSave(vg);
	init_font(vg); // for correct font size

	auto const max_glyphs = str.size();
	// TODO can cache glyphs and update only when text changes
	auto absolute_position = get_position();
	num_glyphs = nvgTextGlyphPositions(vg, X(absolute_position), Y(absolute_position), str.c_str(), nullptr, glyphs.get(), (int)max_glyphs);

	if(cursor_pos > num_glyphs)
		cursor_pos = num_glyphs;

	nvgRestore(vg);
}

void text_edit::update_glyphs_no_bounds()
{
	auto const max_glyphs = str.size();

	glyphs = std::make_unique<NVGglyphPosition[]>(max_glyphs);

	update_glyph_positions();
}

void text_edit::update_glyphs()
{
	update_glyphs_no_bounds();
	update_bounds();
}

void text_edit::set_cursor_to_mouse_pos()
{
	// only need to use X position because it's already known the click is inside the element rectangle, and single-line text is used.
	auto& input_manager = context->input_manager;
	auto const mouse_x = X(input_manager.mouse_pos);

	bool glyph_clicked = false;

	for (auto i = num_glyphs; i-- > 0;)
	{
		auto const& glyph = glyphs[i];
		auto const x_mid = glyph.x + (glyph.maxx - glyph.minx) / 2;

		// check if the glyph was clicked
		if (mouse_x >= glyph.minx &&
			mouse_x <= x_mid) {
			cursor_pos = i;
			glyph_clicked = true;
			break;
		}
		else if (mouse_x >= x_mid &&
					mouse_x <= glyph.maxx) {
			cursor_pos = i+1;
			glyph_clicked = true;
			break;
		}
	}

	// if clicked past the last character, position the cursor at the end of the text
	if(!glyph_clicked) {
		auto const abs_text_end = X(get_position()) + text_bounds[2];
		if(mouse_x > abs_text_end) {
			cursor_pos = str.size();
		}
	}
}

void text_edit::on_mouse_press()
{
	clear_selection();
	set_cursor_to_mouse_pos();

	selection_start_pos = cursor_pos;

	context->input_manager.frame_start.subscribe_global(this, [this]() {
		on_frame_start();
	});
}

void text_edit::on_frame_start()
{
	set_cursor_to_mouse_pos();
	selection_end_pos = cursor_pos;
}

void text_edit::on_key_press(int key, int mods)
{
	switch (key) {
	case GLFW_KEY_BACKSPACE:
		if(has_selection()) {
			delete_selection();
		}
		else if (cursor_pos > 0) {
			str.erase(--cursor_pos, 1);
			update_glyphs();
		}
		break;

	case GLFW_KEY_DELETE:
		if(has_selection()) {
			delete_selection();
		}
		else if (cursor_pos < str.size()) {
			str.erase(cursor_pos, 1);
			update_glyphs();
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
			update_glyphs();
		}
		break;

	case GLFW_KEY_A:
		if(mods & GLFW_MOD_CONTROL) {
			select_all();
		}
		break;
	}
}

void text_edit::on_character(unsigned codepoint)
{
	str.insert(str.begin() + cursor_pos++, codepoint);
	update_glyphs();
	//NOTE: no need to update glyphs when deleting since the deleted glyphs won't be accessed.
}

void text_edit::draw(NVGcontext* vg)
{
	// draw selection background
	if(has_selection())
	{
		if (glyphs == nullptr)
			update_glyphs();

		auto const absolute_position = get_position();

		auto const x_start = selection_start_pos == 0 ?
			X(absolute_position) : // may have no characters, position at the start.
			glyphs[selection_start_pos-1].maxx; // position at the end of the previous character

		auto const x_end = selection_end_pos == 0 ?
			X(absolute_position) :
			glyphs[selection_end_pos-1].maxx;

		nvgBeginPath(vg);
		nvgRect(vg,
				x_start, Y(absolute_position),
				x_end - x_start, Y(size));
		nvgFillColor(vg, selection_color);
		nvgFill(vg);
	}

	text::draw(vg);

	auto& input_manager = context->input_manager;

	// draw cursor
	if (this == input_manager.focused_element)
	{
		if (glyphs == nullptr)
			update_glyphs();

		auto const absolute_position = get_position();

		auto const x_pos = cursor_pos == 0 ?
			X(absolute_position) : // may have no characters, position at the start.
			glyphs[cursor_pos-1].maxx; // position at the end of the previous character

		nvgBeginPath(vg);
		nvgMoveTo(vg, x_pos, Y(absolute_position));
		nvgLineTo(vg, x_pos, Y(absolute_position) + Y(size));
		nvgStrokeColor(vg, nvgRGBA(255, 255, 255, 255));
		nvgStrokeWidth(vg, 1.0f);
		nvgStroke(vg);
	}
}

void text_edit::set_text(std::string const& new_str)
{
	text::set_text(new_str);
	update_glyphs_no_bounds();
}


void text_edit::post_layout()
{
	// glyph positions may change
	update_glyph_positions();
}

bool text_edit::has_selection() const
{
	return selection_start_pos != selection_end_pos;
}

void text_edit::clear_selection()
{
	selection_start_pos = selection_end_pos = 0;
}

void text_edit::delete_selection()
{
	//NOTE: if there's no selection 0 chars will be erased
	auto low_pos = std::min(selection_start_pos, selection_end_pos);
	auto high_pos = std::max(selection_start_pos, selection_end_pos);
	str.erase(low_pos, high_pos - low_pos);
	cursor_pos = low_pos;
	clear_selection();
	update_glyphs();
}

void text_edit::select_all()
{
	selection_start_pos = 0;
	selection_end_pos = str.size();
	cursor_pos = selection_end_pos;
}