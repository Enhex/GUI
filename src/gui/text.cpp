#include "text.h"


text::text()
{
	style = element_name;
	apply_style();
}

void text::setup(int new_font, float new_font_size, std::string const& new_str)
{
	setup(context->vg, new_font, new_font_size, new_str);
}

void text::setup(NVGcontext* vg, int new_font, float new_font_size, std::string const& new_str)
{
	font = new_font;
	font_size = new_font_size;
	set_text(new_str);
}

void text::set_text(std::string const& new_str)
{
	str = new_str;
	update_text();
}

void text::update_text()
{
	update_spans_and_size();
}

void text::span_t::update_glyphs(nx::Vector2 const& absolute_position)
{
	auto const max_glyphs = size();
	glyphs = std::make_unique<NVGglyphPosition[]>(max_glyphs);

	update_glyph_positions(absolute_position);
}

void text::span_t::update_glyph_positions(nx::Vector2 const& absolute_position)
{
	auto const max_glyphs = size();
	num_glyphs = nvgTextGlyphPositions(context->vg, absolute_position.x + offset, absolute_position.y, start, end, glyphs.get(), (int)max_glyphs);
}

void text::update_spans()
{
	spans.clear();
	if(!str.empty()){
		auto abs_pos = get_position();

		// init_font for both tab width and updating spans' glyphs
		auto& vg = context->vg;
		nvgSave(vg);
		init_font(vg); // for correct font size

		// calculate tab width
		NVGglyphPosition glyph;
		nvgTextGlyphPositions(vg, 0, 0, " ", nullptr, &glyph, 1);
		auto const space_width = glyph.maxx;
		tab_width = space_width * 4;

		float offset = 0;
		size_t leading_tabs = 0;

		for(size_t i=0; i < str.size(); ++i){
			if(str[i] == '\t'){
				offset += tab_width;
				++leading_tabs;
			}
			else{
				// start a new span
				auto& span = spans.emplace_back();
				span.start = &str[i];
				// find the span's end
				for(; i < str.size(); ++i){
					auto const& c = str[i];
					if(c == '\t'){
						span.end = &c;
						--i;
						break;
					}
				}
				if(span.end == nullptr){
					span.end = str.data() + str.size();
				}

				span.leading_tabs = leading_tabs;
				leading_tabs = 0;

				// advance the offset by the span's width
				// need to use the span's glyphs (without the tabs as they'll result wrong glyph positions)
				span.offset = offset; // must be set before updating the span's glyphs
				span.update_glyphs(abs_pos);
				auto const span_width = span.glyphs[span.num_glyphs-1].maxx -span.offset - abs_pos.x;
				offset += span_width;
			}
		}

		// if text ends with tabs
		if(leading_tabs != 0){
			auto& span = spans.emplace_back();
			span.leading_tabs = leading_tabs;
			span.offset = offset;
			span.start = str.data() + str.size();
			span.end = span.start;
		}

		nvgRestore(vg);
	}
}

void text::set_style(style::style_t const& style)
{
	auto read = [&](auto& property, std::string&& name)
	{
		using property_t = std::remove_reference_t<decltype(property)>;

		auto iter = style.find(name);
		if (iter != style.end()) {
			property = std::any_cast<property_t>(iter->second);
			return true;
		}
		return false;
	};

	bool bounds_need_update = false;

	std::string font_name;
	if (read(font_name, "font")) {
		font = context->style_manager.font_name_to_id(font_name);
		bounds_need_update = true;
	}

	if(read(font_size, "font_size")) {
		bounds_need_update = true;
	}

	if(bounds_need_update)
		update_spans_and_size();

	read(color, "color");
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

void text::init_font(NVGcontext* vg)
{
	nvgFontSize(vg, font_size);
	nvgFontFaceId(vg, font);
	//nvgTextAlign(vg, NVG_ALIGN_LEFT | NVG_ALIGN_BASELINE);
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