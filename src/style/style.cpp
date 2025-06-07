#include "style.h"
#include <filesystem>

namespace gui::style
{

int manager::create_font(NVGcontext * vg, const char * file_name)
{
	std::filesystem::path path{file_name};

	auto const font_name = path.stem().string();
	auto const font_id = nvgCreateFont(vg, font_name.c_str(), file_name);

	// save mapping from id to name and vice versa
	if (font_id != -1) {
		font_id_to_name_map[font_id] = font_name;
		font_name_to_id_map[font_name] = font_id;
	}

	return font_id;
}

int manager::font_name_to_id(std::string & name)
{
	return font_name_to_id_map.at(name);
}

std::string const & manager::font_id_to_name(int id)
{
	return font_id_to_name_map.at(id);
}

}
