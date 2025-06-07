#pragma once

#include "text_base.h"

namespace gui
{

struct text : text_base
{
	inline static constexpr auto element_name{ "text" };
	std::string get_element_name() noexcept override { return element_name; }

	std::type_info const& type_info() const noexcept override { return typeid(text); }

	std::vector<text_span_t> spans;

	text();

	void update_text() override;

	void update_spans();

	void update_spans_and_size();

	// both size and min_size
	void update_size();

	void draw(NVGcontext* vg) override;
};

}
