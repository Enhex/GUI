#pragma once

#include "element.h"

struct text : element
{
	inline static constexpr auto element_name{ "text" };
	std::string get_element_name() noexcept override { return element_name; }

	std::type_info const& type_info() const noexcept override { return typeid(text); }

	std::string str;
	int font = -1;
	float font_size = 12;
	NVGcolor color = nvgRGBA(255, 255, 255, 255);

	struct span_t{
		const char* start = nullptr;
		const char* end = nullptr; // either tab or string end (one past the last character)
		float offset = 0; // offset in the line relative to its start
		size_t leading_tabs;

		// glyphs' absolute positions
		std::unique_ptr<NVGglyphPosition[]> glyphs;
		int num_glyphs = 0;

		size_t size() const{
			return end - start;
		}

		void update_glyphs(nx::Vector2 const& absolute_position);
		void update_glyph_positions(nx::Vector2 const& absolute_position);
	};

	std::vector<span_t> spans;
	float tab_width = 0;

	text();

	void setup(int new_font, float new_font_size, std::string const& new_str);

	void setup(NVGcontext* vg, int new_font, float new_font_size, std::string const& new_str);

	void set_text(std::string const& new_str);
	virtual void update_text();

	void set_style(style::style_t const& style) override;

	void update_spans();

	void update_spans_and_size();

	// both size and min_size
	void update_size();

	void init_font(NVGcontext* vg);

	void draw(NVGcontext* vg) override;
};