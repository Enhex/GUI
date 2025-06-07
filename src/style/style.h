#pragma once

#include <any>
#include <memory>
#include <nanovg.h>
#include <string>
#include <strong_type.h>
#include <unordered_map>

namespace gui::style
{
	// property name to value
	using style_t = std::unordered_map<std::string, std::any>;

	// strong type style_t for serialization specialization - choose std::any's type according to property name
	STRONG_TYPE(style_st, style_t);

	struct manager
	{
		// map from style name to style
		std::unordered_map<std::string, style_st> styles;


		int create_font(NVGcontext* vg, const char* file_name);

		int font_name_to_id(std::string& name);
		std::string const& font_id_to_name(int id);


	protected:
		std::unordered_map<std::string, int> font_name_to_id_map; //NOTE: NanoVG already got nvgFindFont() which is O(N)
		std::unordered_map<int, std::string> font_id_to_name_map;
	};
}