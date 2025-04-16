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
	update_glyphs_and_size();
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
		update_glyphs_and_size();

	read(color, "color");
}

void text::update_glyph_positions()
{
	auto& vg = context->vg;
	nvgSave(vg);
	init_font(vg); // for correct font size

	auto const max_glyphs = str.size();
	auto const absolute_position = get_position();
	num_glyphs = nvgTextGlyphPositions(vg, absolute_position.x, absolute_position.y, str.c_str(), nullptr, glyphs.get(), (int)max_glyphs);

	nvgRestore(vg);
}

void text::update_glyphs()
{
	auto const max_glyphs = str.size();

	glyphs = std::make_unique<NVGglyphPosition[]>(max_glyphs);

	update_glyph_positions();
}

void text::update_glyphs_and_size()
{
	update_glyphs();
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
		//NOTE: nvgTextBounds doesn't handle spaces at the end of the string correctly. https://github.com/memononen/nanovg/issues/636
		auto const abs_pos = get_position();
		min_size.x = glyphs[str.size()-1].maxx - abs_pos.x;
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
	nvgText(vg, absolute_position.x, absolute_position.y + ascender, str.c_str(), nullptr);
}