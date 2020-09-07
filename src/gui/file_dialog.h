#pragma once

#include "panel.h"
#include <filesystem>

struct scroll_view;
struct text;
struct text_edit;
struct button;

/*

*/
struct file_dialog : panel
{
	inline static constexpr auto element_name{ "file_dialog" };
	std::string get_element_name() override { return element_name; }

	std::type_info const& type_info() const override { return typeid(file_dialog); }

	text& title;
	scroll_view* view;
	text_edit* path_field;
	text_edit* filename_field;
	button* confirm;

	file_dialog();

	// only choose existing files
	void pick_file(std::filesystem::path dir, std::function<void(std::filesystem::path)> callback);
	// can enter new file name
	void save_file(std::filesystem::path dir, std::function<void(std::filesystem::path)> callback);
};