#pragma once

#include "../layout/layout.h"

struct element : layout::element<element>
{
	virtual void draw(NVGcontext* vg) {}
	void draw_recursive(NVGcontext* vg) {
		draw(vg);
		for (auto const& child : children)
			child->draw_recursive(vg);
	}
};

namespace gui::layout
{
	using box = ::layout::box<::element>;
}

struct panel : element
{
	NVGcolor color;

	void draw(NVGcontext* vg) override
	{
		nvgBeginPath(vg);
		nvgRect(vg, X(position), Y(position), X(size), Y(size));
		nvgFillColor(vg, color);
		nvgFill(vg);
	};
};

//TODO set min size to text bounds
struct text : element
{
	std::string str;
	float font_size = 12;
	int font = -1;
	NVGcolor color;

	void draw(NVGcontext* vg) override
	{
		nvgFontSize(vg, font_size);
		nvgFontFaceId(vg, font);
		nvgFillColor(vg, color);
		nvgTextAlign(vg, NVG_ALIGN_LEFT | NVG_ALIGN_TOP);

		float ascender = 0;
		float descender = 0;
		float lineh = 0;
		nvgTextMetrics(vg, &ascender, &descender, &lineh);

		nvgText(vg, X(position), Y(position) + descender, str.c_str(), nullptr);
	}
};