#include "text_span.h"

void text_span_t::update_glyphs(NVGcontext* vg, nx::Vector2 const& absolute_position)
{
	auto const max_glyphs = size();
	glyphs = std::make_unique<NVGglyphPosition[]>(max_glyphs);

	update_glyph_positions(vg, absolute_position);
}

void text_span_t::update_glyph_positions(NVGcontext* vg, nx::Vector2 const& absolute_position)
{
	auto const max_glyphs = size();
	num_glyphs = nvgTextGlyphPositions(vg, absolute_position.x + offset, absolute_position.y, start, end, glyphs.get(), (int)max_glyphs);
}
