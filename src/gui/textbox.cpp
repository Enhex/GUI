#include "textbox.h"

textbox::textbox()
{
	style = element_name;
}

void textbox::update_text()
{
	update_bounds();
}

void textbox::update_glyph_positions()
{
	auto& vg = context->vg;
	nvgSave(vg);
	init_font(vg); // for correct font size

	auto const max_glyphs = str.size();
	auto const absolute_position = get_position();
	num_glyphs = nvgTextGlyphPositions(vg, absolute_position.x, absolute_position.y, str.c_str(), nullptr, glyphs.get(), (int)max_glyphs);

	// nvgTextGlyphPositions only works with single line so characters in a new line got wrong X position as if they're all in a single line,
	// so fix the positions.
	//NOTE: can't edit positions directly otherwise nanovg crashes.
	//NOTE: starting from i=1 because first line doesn't need offsetting, except the newline of the first line.
	glyph_offsets.resize(max_glyphs);
	// offset the first line's newline
	if(rows.size() > 1) { // at least 1 line and it isn't the last one
		auto const glyph_x_offset = glyphs[rows[0].start - str.data()].minx - absolute_position.x;
		glyph_offsets[rows[0].end - str.data()] = glyph_x_offset;
	}
	// offset the rest of the lines
	for (size_t i=0; i < rows.size(); ++i)
	{
		auto const& row = rows[i];
		auto const glyph_x_offset = glyphs[row.start - str.data()].minx - absolute_position.x;
		for(auto p = row.start; p < row.end; ++p) {
			glyph_offsets[p - str.data()] = glyph_x_offset;
		}
		// newline also gets a glyph, excluding the last row since it doesn't have newline.
		//last row actually doesn't have a newline?
		if(i < rows.size()-1) {
			glyph_offsets[row.end - str.data()] = glyph_x_offset;
		}
	}

	nvgRestore(vg);
}

void textbox::update_glyphs_no_bounds()
{
	auto const max_glyphs = str.size();

	glyphs = std::make_unique<NVGglyphPosition[]>(max_glyphs);

	update_glyph_positions();
}

void textbox::update_glyphs()
{
	update_glyphs_no_bounds();
	update_bounds();
}

void textbox::update_rows()
{
	init_font(context->vg);
	float ascender;
	nvgTextMetrics(context->vg, &ascender, nullptr, nullptr);
	auto absolute_position = get_position();

	rows.clear();
	if(!str.empty()) {
		auto* current_row = &rows.emplace_back();
		current_row->start = &str[0];

		for(auto& c : str) {
			if(c == '\n') {
				current_row->end = &c;
				// add new row if there are more characters
				if(&c != &str.back()) {
					current_row = &rows.emplace_back();
					current_row->start = &c + 1;
				}
			}
		}
		auto const str_end = (&str.back()) + 1;
		// if string ending with newline add an empty row
		if(str.back() == '\n') {
			auto& last_row = rows.emplace_back();
			last_row.start = str_end;
			last_row.end = str_end;
		}
		else {
			current_row->end = str_end;
		}
	}
}

void textbox::update_bounds()
{
	update_rows();
	update_glyphs_no_bounds();

	auto& vg = context->vg;
	nvgSave(vg);

	init_font(vg);

	float ascender, descender, lineh;
	nvgTextMetrics(vg, &ascender, &descender, &lineh);
	if(str.empty()) {
		size.y = min_size.y = lineh;
	}
	else {
		auto const abs_pos = get_position();
		min_size.x = glyphs[0].maxx - glyph_offsets[0] - abs_pos.x;

		for(auto const& row : rows) {
			// if it's an empty row its width is 0
			if(row.size() == 0)
				continue;

			auto const glyph_index = row.end-1 - str.data();
			auto const row_width = glyphs[glyph_index].maxx - glyph_offsets[glyph_index] - abs_pos.x;
			// using local space with position 0, can directly use max point instead of calculating bounding box size
			if(min_size.x < row_width)
				min_size.x = row_width;
		}

		min_size.y = lineh * rows.size();

		size = min_size;
	}

	nvgRestore(vg);
}

void textbox::draw(NVGcontext* vg)
{
	init_font(vg);
	nvgFillColor(vg, color);

	float ascender, descender, lineh;
	nvgTextMetrics(vg, &ascender, &descender, &lineh);
	auto absolute_position = get_position();
	for(size_t i=0; i < rows.size(); ++i) {
		auto const& row = rows[i];
		auto const y = i * lineh + absolute_position.y + ascender;
		nvgText(vg, absolute_position.x, y, row.start, row.end);
	}
}