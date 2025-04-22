#pragma once

#include "../math/Vector2.h"

struct text_span_t{
	const char* start = nullptr;
	const char* end = nullptr; // either tab or string end (one past the last character)
	float offset = 0; // offset in the line relative to its start
	size_t leading_tabs;

	// glyphs' absolute positions
	std::unique_ptr<NVGglyphPosition[]> glyphs;
	int num_glyphs = 0; //TODO always equals to size() thus can be replaced with it?

	size_t size() const{
		return end - start;
	}

	void update_glyphs(NVGcontext* vg, nx::Vector2 const& absolute_position);
	void update_glyph_positions(NVGcontext* vg, nx::Vector2 const& absolute_position);
};

using spans_t = std::vector<text_span_t>;
