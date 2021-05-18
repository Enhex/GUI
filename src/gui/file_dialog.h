#pragma once

#include "panel.h"

#include <filesystem>

struct scroll_view;
struct text;
struct text_edit;
struct button;

/*
widget for selecting files
*/
struct file_dialog : panel
{
	inline static constexpr auto element_name{ "file_dialog" };
	std::string get_element_name() override { return element_name; }

	std::type_info const& type_info() const override { return typeid(file_dialog); }

	// use an ancestor element's visibility for showing/hiding the dialog
	// if null `this` will be used
	element* visibility_element = nullptr;

	text& title;
	element& filename_container;
	scroll_view* view;
	text_edit* path_field;
	text_edit* filename_field;
	button* confirm;

	file_dialog();

	void set_visible(bool visible);

	// only choose existing files
	// extension: filter displayed files
	void pick_file(std::filesystem::path dir, std::function<void(std::filesystem::path)> callback, std::vector<std::filesystem::path> const& extensions = {});
	void pick_file(std::filesystem::path dir, std::function<void(std::filesystem::path)> callback, std::filesystem::path extension);
	// can enter new file name
	// extension: filter displayed files and save file with the extension
	void save_file(std::filesystem::path dir, std::function<void(std::filesystem::path)> callback, std::filesystem::path extension = "");
};