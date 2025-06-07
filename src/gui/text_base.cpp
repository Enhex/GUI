#include "text_base.h"

namespace gui
{

text_base::text_base()
{
	style = "text";
}

void text_base::setup(int new_font, float new_font_size, std::string const& new_str)
{
	setup(context->vg, new_font, new_font_size, new_str);
}

void text_base::setup(NVGcontext* vg, int new_font, float new_font_size, std::string const& new_str)
{
	font = new_font;
	font_size = new_font_size;
	set_text(new_str);
}

void text_base::set_text(std::string const& new_str)
{
	str = new_str;
	update_text();
}

void text_base::update_spans(std::string_view const str, spans_t& spans, nx::Vector2 const& abs_pos)
{
	spans.clear();
	if(!str.empty()){
		// calculate tab width
		NVGglyphPosition glyph;
		auto& vg = context->vg;
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
				span.update_glyphs(vg, abs_pos);
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
	}
}

void text_base::set_style(style::style_t const& style)
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
		update_text();

	read(color, "color");
}

void text_base::init_font(NVGcontext* vg)
{
	nvgFontSize(vg, font_size);
	nvgFontFaceId(vg, font);
	//nvgTextAlign(vg, NVG_ALIGN_LEFT | NVG_ALIGN_BASELINE);
}

}
