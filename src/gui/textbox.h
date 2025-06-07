#pragma once

#include "text_base.h"

namespace gui
{

/* a multi-line text.
unlike single line text that updates its size based on the text, here the text may auto-split
across lines so the size isn't derived from the text.
*/
struct textbox : text_base
{
	struct Row {
		const char* start = nullptr;
		const char* end = nullptr; // either newline or string end (one past the last character)
		std::vector<text_span_t> spans;

		size_t size() const
		{
			return end - start;
		}
	};

	inline static constexpr auto element_name{ "textbox" };
	std::string get_element_name() noexcept override { return element_name; }

	std::type_info const& type_info() const noexcept override { return typeid(textbox); }

	std::vector<Row> rows;

	textbox();

	void update_text() override;

	void update_spans_and_size();

	void update_rows();
	void update_spans(); // must be done after update_rows()
	void update_size();
	void draw(NVGcontext* vg) override;
};

}
