#include "scissor.h"

// scissors child elements
void scissor::draw_recursive(NVGcontext* vg)
{
	nvgSave(vg);
	auto absolute_position = get_position();
	nvgIntersectScissor(vg, X(absolute_position), Y(absolute_position), X(size), Y(size));

	for (auto const& child : children)
		child->draw_recursive(vg);

	// using save and restore to handle nested scissor elements, so one a child scissor is done it will restore its parent's scissor.
	nvgRestore(vg);
}