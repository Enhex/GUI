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
	enum Mode : uint_fast8_t {
		none,
		pick,
		save
	};

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
	text_edit* folder_field;
	text_edit* filename_field;
	button* confirm;
	button* add_folder;
	element* folder_dialog;

	file_dialog();

	void set_visible(bool visible);

	// only choose existing files
	// extension: filter displayed files
	void pick_file(std::filesystem::path dir, std::function<void(std::filesystem::path)> callback, std::vector<std::filesystem::path> const& extensions = {});
	void pick_file(std::filesystem::path dir, std::function<void(std::filesystem::path)> callback, std::filesystem::path extension);
	void pick_change_dir(std::filesystem::path const& dir);
	// can enter new file name
	// extension: filter displayed files and save file with the extension
	void save_file(std::filesystem::path dir, std::function<void(std::filesystem::path)> callback, std::filesystem::path extension = "");
	void save_change_dir(std::filesystem::path const& dir);

protected:
	std::function<void(std::filesystem::path)> current_callback;
	std::vector<std::filesystem::path> current_extensions;
	std::filesystem::path current_extension;

	std::filesystem::path current_dir;

	size_t paths_count = 0;
	float original_min_size;
	Mode mode = Mode::none;

	button& create_path(std::filesystem::path const& path, std::string str = "");
	void add_path_pick(std::filesystem::path const& path, std::string str = "");
	void add_path_save(std::filesystem::path const& path, std::string str = "");

	void toggle_add_folder_dialog(bool show);
	void confirm_folder_dialog();
	void refresh();
};