#pragma once

#include "element.h"

struct panel : element
{
	std::type_info const& type_info() const override { return typeid(panel); }

	NVGcolor color;

	void draw(NVGcontext* vg) override
	{
		nvgBeginPath(vg);
		nvgRect(vg, X(position), Y(position), X(size), Y(size));
		nvgFillColor(vg, color);
		nvgFill(vg);
	};
};