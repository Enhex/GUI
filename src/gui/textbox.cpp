#include "textbox.h"

textbox::textbox()
{
	style = element_name;
}

void textbox::update_text()
{
	update_rows();
	update_glyphs_and_size();
}

void textbox::update_glyph_positions()
{
	num_glyphs = 0;
	if(rows.empty()){
		return;
	}

	auto& vg = context->vg;
	nvgSave(vg);
	init_font(vg); // for correct font size

	// nvgTextGlyphPositions only works with single line so characters in a new line got wrong X position as if they're all in a single line,
	// so call it for each line separately
	auto const absolute_position = get_position();
	size_t glpyhs_i = 0; // offset of each row
	for(size_t i=0; i < rows.size()-1; ++i){
		auto const& row = rows[i];
		auto const chars_count = row.size()+1;
		num_glyphs += nvgTextGlyphPositions(vg, absolute_position.x, absolute_position.y, row.start, row.end+1, glyphs.get()+glpyhs_i, (int)chars_count);
		glpyhs_i += chars_count;
	}
	// last row doesn't have a newline at the end
	{
		auto const& row = rows.back();
		num_glyphs += nvgTextGlyphPositions(vg, absolute_position.x, absolute_position.y, row.start, row.end, glyphs.get()+glpyhs_i, (int)row.size());
	}

	nvgRestore(vg);
}

void textbox::update_glyphs()
{
	auto const max_glyphs = str.size();

	glyphs = std::make_unique<NVGglyphPosition[]>(max_glyphs);

	update_glyph_positions();
}

void textbox::update_glyphs_and_size()
{
	update_glyphs();
	update_size();
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
		auto const str_end = str.data() + str.size();
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

void textbox::update_size()
{
	auto& vg = context->vg;
	nvgSave(vg);

	init_font(vg);

	float ascender, descender, lineh;
	nvgTextMetrics(vg, &ascender, &descender, &lineh);
	if(str.empty()) {
		size.y = min_size.y = lineh;
	}
	else {
		min_size.x = glyphs[0].maxx - glyphs[0].minx;

		for(auto const& row : rows) {
			// if it's an empty row its width is 0
			if(row.size() == 0)
				continue;

			auto const last_glyph_i = row.end-1 - str.data();
			auto const first_glyph_i = row.start - str.data();
			auto const row_width = glyphs[last_glyph_i].maxx - glyphs[first_glyph_i].minx;;
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