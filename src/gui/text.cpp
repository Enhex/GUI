#include "text.h"

text::text()
{
	style = element_name;
	apply_style();
}

void text::update_text()
{
	update_spans_and_size();
}

void text::update_spans()
{
	// init_font for both tab width and updating spans' glyphs
	auto& vg = context->vg;
	nvgSave(vg);
	init_font(vg); // for correct font size

	auto const abs_pos = get_position();
	text_base::update_spans(str, spans, abs_pos);

	nvgRestore(vg);
}

void text::update_spans_and_size()
{
	update_spans();
	update_size();
}

void text::update_size()
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
		//NOTE: nvgTextBounds doesn't handle spaces at the end of the string correctly. https://github.com/memononen/nanovg/issues/636 https://github.com/memononen/nanovg/issues/225
		auto const abs_pos = get_position();
		auto const& span = spans.back();
		if(span.size() == 0){
			min_size.x = span.offset;
		}
		else{
			min_size.x = span.glyphs[span.size()-1].maxx - abs_pos.x;
		}
		min_size.y = lineh;

		size = min_size;
	}

	nvgRestore(vg);
}

void text::draw(NVGcontext* vg)
{
	init_font(vg);
	nvgFillColor(vg, color);

	float ascender, descender, lineh;
	nvgTextMetrics(vg, &ascender, &descender, &lineh);
	auto absolute_position = get_position();
	for(auto const& span : spans){
		nvgText(vg, absolute_position.x + span.offset, absolute_position.y + ascender, span.start, span.end);
	}
}