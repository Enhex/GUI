#include "textbox.h"

textbox::textbox()
{
	style = element_name;
}

void textbox::update_bounds()
{
	if(fixed_size)
		return;

	auto& vg = context->vg;
	nvgSave(vg);

	init_font(vg);

	float ascender, descender, lineh;
	nvgTextMetrics(vg, &ascender, &descender, &lineh);
	if(str.empty()) {
		Y(size) = Y(min_size) = lineh;
	}
	else {
		nvgTextBoxBounds(vg, 0, 0, INFINITY, str.c_str(), nullptr, text_bounds);

		// using local space with position 0, can directly use max point instead of calculating bounding box size
		X(min_size) = text_bounds[2];
		Y(min_size) = text_bounds[3] + ascender;

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
	nvgTextBox(vg, X(absolute_position), Y(absolute_position) + ascender, X(size), str.c_str(), nullptr);
}