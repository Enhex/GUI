#include "textbox.h"

namespace gui
{

textbox::textbox()
{
	apply_style();
}

void textbox::update_text()
{
	update_rows();
	update_spans_and_size();
}

void textbox::update_spans_and_size()
{
	update_spans();
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

void textbox::update_spans()
{
	// init_font for both tab width and updating spans' glyphs
	auto& vg = context->vg;
	nvgSave(vg);
	init_font(vg); // for correct font size

	auto const abs_pos = get_position();
	for(auto& row : rows){
		std::string_view str(row.start, row.size());
		text_base::update_spans(str, row.spans, abs_pos);
	}

	nvgRestore(vg);
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
		auto const abs_pos = get_position();
		auto const& first_span = rows[0].spans[0];
		if(first_span.size() == 0){
			min_size.x = first_span.offset;
		}
		else{
			min_size.x = first_span.glyphs[0].maxx - abs_pos.x;
		}

		for(auto const& row : rows) {
			// if it's an empty row its width is 0
			if(row.size() == 0)
				continue;

			float row_width = 0;
			auto const& last_span = row.spans.back();
			if(last_span.size() == 0){
				row_width = last_span.offset;
			}
			else{
				row_width = last_span.glyphs[last_span.size()-1].maxx - abs_pos.x;
			}
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
		for(auto const& span : row.spans){
			nvgText(vg, absolute_position.x + span.offset, y, span.start, span.end);
		}
	}
}

}
