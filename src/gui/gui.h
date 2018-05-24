#pragma once

#include "../input/input.h"
#include "../layout/layout.h"

struct element : layout::element<element>, input::element
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
	NVGcolor color = nvgRGBA(255,255,255,255);

	void update_bounds(NVGcontext* vg)
	{
		nvgSave(vg);

		init_font(vg);

		float bounds[4];
		nvgTextBounds(vg, 0, 0, str.c_str(), nullptr, bounds);
		
		float ascender, descender, lineh;
		nvgTextMetrics(vg, &ascender, &descender, &lineh);

		// using local space with position 0, can directly use max point instead of calculating bounding box size
		X(size) = X(min_size) = bounds[2];
		Y(size) = Y(min_size) = bounds[3] + ascender;


		nvgRestore(vg);
	}

	void init_font(NVGcontext* vg)
	{
		nvgFontSize(vg, font_size);
		nvgFontFaceId(vg, font);
		//nvgTextAlign(vg, NVG_ALIGN_LEFT | NVG_ALIGN_BASELINE);
	}

	void draw(NVGcontext* vg) override
	{
		init_font(vg);
		nvgFillColor(vg, color);

		float ascender, descender, lineh;
		nvgTextMetrics(vg, &ascender, &descender, &lineh);
		nvgText(vg, X(position), Y(position) + ascender, str.c_str(), nullptr);

		update_bounds(vg);
	}
};
