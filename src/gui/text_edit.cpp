#include "text_edit.h"

#include "../include_glfw.h"
#include "../framework/application.h"

text_edit::text_edit()
{
	style = element_name;

	auto& input_manager = context->input_manager;

	input_manager.subscribe<input::event::mouse_press>(this, [this](std::any&& args) {
		on_mouse_press();
	});
	input_manager.subscribe<input::event::key_press>(this, [this](std::any&& args) {
		auto&[key, mods] = std::any_cast<input::event::key_press::params&>(args);
		on_key_press(key, mods);
	});
	input_manager.subscribe<input::event::key_repeat>(this, [this](std::any&& args) {
		auto&[key, mods] = std::any_cast<input::event::key_repeat::params&>(args);
		on_key_press(key, mods);
	});
	input_manager.subscribe<input::event::character>(this, [this](std::any&& args) {
		auto&[codepoint] = std::any_cast<input::event::character::params&>(args);
		on_character(codepoint);
	});

	// subscribe so text_edit will be able to capture focus
	input_manager.subscribe<input::event::focus_start>(this, [this](std::any&& args) {});
}

void text_edit::on_mouse_press()
{
	// only need to use X position because it's already known the click is inside the element rectangle, and single-line text is used.

	auto absolute_position = get_position();

	bool glyph_clicked = false;

	auto& input_manager = context->input_manager;
	auto const mouse_x = X(input_manager.mouse_pos);

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
		if(mouse_x > text_bounds[2]) {
			cursor_pos = str.size();
		}
	}
}

void text_edit::on_key_press(int key, int mods)
{
	switch (key) {
	case GLFW_KEY_BACKSPACE:
		if (cursor_pos > 0)
			str.erase(--cursor_pos, 1);
			update_glyphs();
		break;

	case GLFW_KEY_DELETE:
		if (cursor_pos < str.size())
			str.erase(cursor_pos, 1);
			update_glyphs();
		break;

	case GLFW_KEY_LEFT:
		if (cursor_pos > 0)
			--cursor_pos;
		break;

	case GLFW_KEY_RIGHT:
		if (cursor_pos < str.size())
			++cursor_pos;
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
	//TODO draw selection background

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
	update_glyphs<true, true>();
}


void text_edit::post_layout()
{
	update_glyphs<false>();
}