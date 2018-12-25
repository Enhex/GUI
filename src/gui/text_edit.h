#pragma once

#include "../include_glfw.h"
#include "text.h"

// editable text
/*TODO
- draw selection rectangle before drawing text
- draw cursor(caret) after drawing text
*/
struct text_edit : text
{
	inline static constexpr auto element_name{ "text_edit" };
	std::string get_element_name() override { return element_name; }

	std::type_info const& type_info() const override { return typeid(text_edit); }

	text_edit()
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
	}

	size_t cursor_pos = 0;

	std::unique_ptr<NVGglyphPosition[]> glyphs;

	int update_glyphs()
	{
		auto& vg = context->vg;

		nvgSave(vg);

		init_font(vg); // for correct font size

		auto const max_glyphs = str.size();
		glyphs = std::make_unique<NVGglyphPosition[]>(max_glyphs);
		// TODO can cache glyphs and update only when text changes
		auto const num_glyphs = nvgTextGlyphPositions(vg, X(position), Y(position), str.c_str(), nullptr, glyphs.get(), (int)max_glyphs);
		//auto const num_glyphs = nvgTextGlyphPositions(vg, 0,0, str.c_str(), nullptr, glyphs.get(), (int)max_glyphs);

		nvgRestore(vg);

		return num_glyphs;
	}

	void on_mouse_press()
	{
		// only need to use X position because it's already known the click is inside the element rectangle, and single-line text is used.

		auto const num_glyphs = update_glyphs();

		for (auto i = num_glyphs; i-- > 0;)
		{
			auto const& glyph = glyphs[i];
			auto const x_min = glyphs[i].x; // current glyph start position
			auto const x_max = i + 1 < num_glyphs ? glyphs[i + 1].x : X(position) + X(size); // next glyph start position

			auto& input_manager = context->input_manager;

			// check if the glyph was clicked
			if (X(input_manager.mouse_pos) >= x_min &&
				X(input_manager.mouse_pos) <= x_max) {
				cursor_pos = i;
				break;
			}
		}
	}

	void on_key_press(int key, int mods)
	{
		switch (key) {
		case GLFW_KEY_BACKSPACE:
			if (cursor_pos > 0)
				str.erase(--cursor_pos, 1);
			break;

		case GLFW_KEY_DELETE:
			if (cursor_pos < str.size())
				str.erase(cursor_pos, 1);
			break;

		case GLFW_KEY_LEFT:
			if (cursor_pos > 0)
				--cursor_pos;
			break;

		case GLFW_KEY_RIGHT:
			if (cursor_pos < str.size())
				++cursor_pos;
			break;
		}
	}

	void on_character(unsigned codepoint)
	{
		str.insert(str.begin() + cursor_pos++, codepoint);
		update_glyphs();
		//NOTE: no need to update glyphs when deleting since the deleted glyphs won't be accessed.
	}

	void draw(NVGcontext* vg) override
	{
		//TODO draw selection background

		text::draw(vg);

		auto& input_manager = context->input_manager;

		// draw cursor
		if (this == input_manager.focused_element)
		{
			if (glyphs == nullptr)
				update_glyphs();

			auto const x_pos = cursor_pos == str.size() ? X(position) + X(size) : glyphs[cursor_pos].x;

			nvgBeginPath(vg);
			nvgMoveTo(vg, x_pos, Y(position));
			nvgLineTo(vg, x_pos, Y(position) + Y(size));
			nvgStrokeColor(vg, nvgRGBA(255, 255, 255, 255));
			nvgStrokeWidth(vg, 1.0f);
			nvgStroke(vg);
		}
	}
};