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
	input_manager.mouse_release.subscribe_global_unfocused(this, [this](int button, int mods) {
		//NOTE: frame_start only sends global events
		context->input_manager.frame_start.unsubscribe_global(this);
	});
	input_manager.double_click.subscribe(this, [this](int button, int mods) {
		on_double_click();
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

	// subscribe so text_edit will be able to capture focus
	input_manager.focus_start.subscribe(this, [this]() {});

	input_manager.mouse_release.set_continue_propagating(this, true);
}

void text_edit::on_str_changed()
{
	text_edit_shared::on_str_changed(str.size());
	update_spans_and_size();
	on_text_changed();
}

void text_edit::update_glyph_positions()
{
	auto const abs_pos = get_position();
	for(auto& span : spans){
		span.update_glyph_positions(abs_pos);
	}

	if(cursor_pos > str.size())
		set_cursor_pos(str.size());
}

void text_edit::set_cursor_to_mouse_pos()
{
	// only need to use X position because it's already known the click is inside the element rectangle, and single-line text is used.
	auto& input_manager = context->input_manager;
	auto const mouse_x = input_manager.mouse_pos.x;

	size_t pos = 0;
	bool glyph_clicked = false;
	// check if the glyph was clicked
	auto check_clicked = [&](float x_min, float x_max, float x_mid){
		if(mouse_x >= x_min &&
		   mouse_x <= x_mid)
		{
			set_cursor_pos(pos);
			glyph_clicked = true;
			return true;
		}
		else if(mouse_x >= x_mid &&
		        mouse_x <= x_max)
		{
			set_cursor_pos(pos+1);
			glyph_clicked = true;
			return true;
		}
		return false;
	};

	auto const abs_pos = get_position();

	for(auto const& span : spans){
		// tabs
		for(size_t i=1; i <= span.leading_tabs; ++i){
			auto const x_min = (abs_pos.x + span.offset) - tab_width * i;
			auto const x_max = x_min + tab_width;
			auto const x_mid = x_min + (tab_width / 2);
			if(check_clicked(x_min, x_max, x_mid)){
				goto label_clicked;
			}
			++pos;
		}
		// glyphs
		for(size_t i=0; i < span.num_glyphs; ++i){
			auto const& glyph = span.glyphs[i];
			auto const x_mid = glyph.x + (glyph.maxx - glyph.minx) / 2;
			if(check_clicked(glyph.minx, glyph.maxx, x_mid)){
				goto label_clicked;
			}
			++pos;
		}
	}

	label_clicked:

	// if clicked past the last character, position the cursor at the end of the text
	if(!glyph_clicked) {
		auto const abs_text_end = get_position().x + size.x;
		if(mouse_x > abs_text_end) {
			set_cursor_pos(str.size());
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

void text_edit::on_double_click()
{
	if(cursor_pos != last_cursor_pos){
		return;
	}

	// check if didn't start selecting already
	if(selection_start_pos == selection_end_pos && !str.empty()){
		// if double clicked whitespace select it and all the adjacent whitespaces
		if(str[cursor_pos] == ' ')
		{
			bool found = false;
			for(selection_start_pos = cursor_pos; selection_start_pos > 0; --selection_start_pos){
				if(str[selection_start_pos] != ' '){
					++selection_start_pos; // exclude whitespace
					found = true;
					break;
				}
			}
			if(!found){
				// for loop ends at index 1, but if the first character is whitespace start should be 0.
				if(str[0] == ' '){
					selection_start_pos = 0;
				}
			}

			auto const size = str.size();
			selection_end_pos = size;
			for(auto i = cursor_pos; i < size; ++i){
				if(str[i] != ' '){
					selection_end_pos = i;
					break;
				}
			}
		}
		// find previous and next whitespaces and select the text between them
		else{
			bool found = false;
			for(selection_start_pos = cursor_pos; selection_start_pos > 0; --selection_start_pos){
				if(str[selection_start_pos] == ' '){
					++selection_start_pos; // exclude whitespace
					found = true;
					break;
				}
			}
			if(!found){
				// for loop ends at index 1, but if the first character is not whitespace start should be 0.
				if(str[0] != ' '){
					selection_start_pos = 0;
				}
			}

			auto const size = str.size();
			selection_end_pos = size;
			for(auto i = cursor_pos; i < size; ++i){
				if(str[i] == ' '){
					selection_end_pos = i;
					break;
				}
			}
		}

		set_cursor_pos(selection_end_pos);
	}
}

void text_edit::on_key_press(int key, int mods)
{
	switch (key) {
	case GLFW_KEY_BACKSPACE:
		if(has_selection()) {
			delete_selection();
		}
		else if (cursor_pos > 0) {
			set_cursor_pos(cursor_pos - 1);
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
		auto const select = shared_select_code(mods);
		if(cursor_pos > 0){
			set_cursor_pos(cursor_pos - 1);

			if(select)
				selection_end_pos = cursor_pos;
		}
		break;
	}
	case GLFW_KEY_RIGHT:
	{
		auto const select = shared_select_code(mods);
		if(cursor_pos < str.size()){
			set_cursor_pos(cursor_pos + 1);

			if(select)
				selection_end_pos = cursor_pos;
		}
		break;
	}
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
			delete_selection();
			auto& app = static_cast<application&>(*context);
			auto const cstr = glfwGetClipboardString(app.window);
			str.insert(cursor_pos, cstr);
			set_cursor_pos(cursor_pos + strlen(cstr));
			on_str_changed();
		}
		break;

	case GLFW_KEY_A:
		if(mods & GLFW_MOD_CONTROL) {
			select_all();
		}
		break;

	case GLFW_KEY_TAB:
		on_character('\t');
		break;
	}
}

void text_edit::on_character(unsigned codepoint)
{
	if(!is_valid_character(codepoint))
		return;

	if(has_selection()) {
		delete_selection();
	}
	str.insert(str.begin() + cursor_pos, codepoint);
	set_cursor_pos(cursor_pos + 1);
	on_str_changed();
	//NOTE: no need to update glyphs when deleting since the deleted glyphs won't be accessed.
}

float text_edit::get_char_pos_x(size_t char_pos, nx::Vector2 const& absolute_position)
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
	// error: char_pos is outside string bounds
	return 0;
}

void text_edit::draw(NVGcontext* vg)
{
	// draw selection background
	if(has_selection())
	{
		update_spans_and_size();

		auto const absolute_position = get_position();
		auto const x_start = get_char_pos_x(selection_start_pos, absolute_position);
		auto const x_end = get_char_pos_x(selection_end_pos, absolute_position);

		nvgBeginPath(vg);
		nvgRect(vg,
				x_start, absolute_position.y,
				x_end - x_start, size.y);
		nvgFillColor(vg, selection_color);
		nvgFill(vg);
	}

	text::draw(vg);

	auto& input_manager = context->input_manager;

	// draw cursor
	if (this == input_manager.focused_element)
	{
		update_spans_and_size();

		auto const absolute_position = get_position();
		auto const x_pos = get_char_pos_x(cursor_pos, absolute_position);

		nvgBeginPath(vg);
		nvgMoveTo(vg, x_pos, absolute_position.y);
		nvgLineTo(vg, x_pos, absolute_position.y + size.y);
		nvgStrokeColor(vg, nvgRGBA(255, 255, 255, 255));
		nvgStrokeWidth(vg, 1.0f);
		nvgStroke(vg);
	}
}

void text_edit::update_text()
{
	clear_selection();
	on_str_changed();
}


void text_edit::post_layout()
{
	// glyph positions may change
	update_glyph_positions();
}

void text_edit::delete_selection()
{
	//NOTE: if there's no selection 0 chars will be erased
	auto low_pos = std::min(selection_start_pos, selection_end_pos);
	auto high_pos = std::max(selection_start_pos, selection_end_pos);
	str.erase(low_pos, high_pos - low_pos);
	set_cursor_pos(low_pos);
	clear_selection();
	on_str_changed();
}

void text_edit::select_all()
{
	selection_start_pos = 0;
	selection_end_pos = str.size();
	set_cursor_pos(selection_end_pos);
}

void text_edit::move_cursor_to_end()
{
	set_cursor_pos(str.size());
}