#pragma once

#include "text.h"

/* a multi-line text.
unlike single line text that updates its size based on the text, here the text may auto-split
across lines so the size isn't derived from the text.
*/
struct textbox : text
{
	struct Row {
		const char* start = nullptr;
		const char* end = nullptr; // either newline or string end (one past the last character)

		size_t size() const
		{
			return end - start;
		}
	};

	inline static constexpr auto element_name{ "textbox" };
	std::string get_element_name() noexcept override { return element_name; }

	std::type_info const& type_info() const noexcept override { return typeid(textbox); }

	std::vector<Row> rows;

	// glyphs' absolute positions
	std::unique_ptr<NVGglyphPosition[]> glyphs;
	int num_glyphs = 0;

	textbox();

	void update_text() override;

	void update_glyph_positions();
	void update_glyphs();
	void update_glyphs_and_size();

	void update_rows();
	void update_size();
	void draw(NVGcontext* vg) override;
};