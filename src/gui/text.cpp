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
	update_bounds();
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
		update_bounds();

	read(color, "color");
}

void text::update_bounds()
{
	auto& vg = context->vg;
	nvgSave(vg);

	init_font(vg);

	float ascender, descender, lineh;
	nvgTextMetrics(vg, &ascender, &descender, &lineh);
	if(str.empty()) {
		Y(size) = Y(min_size) = lineh;
	}
	else {
		nvgTextBounds(vg, 0, 0, str.c_str(), nullptr, text_bounds);

		// using local space with position 0, can directly use max point instead of calculating bounding box size
		X(min_size) = text_bounds[2];
		Y(min_size) = text_bounds[3] + ascender;

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
	nvgText(vg, X(absolute_position), Y(absolute_position) + ascender, str.c_str(), nullptr);
}