#pragma once

#include "panel.h"
#include <filesystem>

struct scroll_view;
struct text;

/*

*/
struct file_dialog : panel
{
	inline static constexpr auto element_name{ "file_dialog" };
	std::string get_element_name() override { return element_name; }

	std::type_info const& type_info() const override { return typeid(file_dialog); }

	text& title;
	scroll_view* view;

	file_dialog();

	//
	void pick_file(std::filesystem::path dir, std::function<void(std::filesystem::path)> callback);
};