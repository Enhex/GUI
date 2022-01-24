#include "file_dialog.h"

#include "../include_glfw.h"
#include "../input/events.h"
#include "button.h"
#include "scroll_view.h"
#include "text.h"
#include "text_edit.h"

namespace fs = std::filesystem;

file_dialog::file_dialog() :
	title(create_child<text>()),
	filename_container(create_child<element>())
{
	color = NVGcolor{0.2f,0.2f,0.2f,1.f};

	style = element_name;
	apply_style();

	create_layout<gui::layout::box>();

	// elements
	{
		filename_container.create_layout<gui::layout::box>().orient = layout::horizontal;
		filename_container.expand[layout::horizontal] = true;

		auto& txt = filename_container.create_child<text>();
		txt.set_text("file name: ");

		auto& bg = filename_container.create_child<panel>();
		bg.create_layout<gui::layout::box>();
		bg.expand[layout::horizontal] = true;
		bg.color = NVGcolor{0.15,0.15,0.15,1};

		filename_field = &bg.create_child<text_edit>();
		filename_field->expand[layout::horizontal] = true;
	}
	{
		auto& border = create_child<panel>();
		border.min_size = {0,1};
		border.expand[layout::horizontal] = true;
	}

	{
		auto& container = create_child<element>();
		container.create_layout<gui::layout::box>().orient = layout::horizontal;
		container.expand[layout::horizontal] = true;
		{
			auto& bg = container.create_child<panel>();
			bg.create_layout<gui::layout::box>();
			bg.expand[layout::horizontal] = true;
			bg.color = NVGcolor{0.15,0.15,0.15,1};

			path_field = &bg.create_child<text_edit>();
			path_field->expand[layout::horizontal] = true;

			folder_field = &bg.create_child<text_edit>();
			folder_field->expand[layout::horizontal] = true;
			folder_field->set_visible(false);
			// pressing enter confirms the name
			context->input_manager.key_press.subscribe(folder_field, [this](int key, int mods) {
				folder_field->on_key_press(key, mods);//TODO need a way to additively subscribe to events

				if(key == GLFW_KEY_ENTER)
					confirm_folder_dialog();
			});
		}
		{
			auto& spacer = container.create_child<element>();
			X(spacer.min_size) = 32;
		}
		{
			add_folder = &container.create_child<button>();
			auto& add_folder_txt = add_folder->create_child<text>();
			add_folder_txt.set_text("add folder");
			add_folder->callback = [this]{
				toggle_add_folder_dialog(true);
			};
		}
		{
			folder_dialog = &container.create_child<element>();
			folder_dialog->create_layout<gui::layout::box>().orient = layout::horizontal;
			folder_dialog->set_visible(false);

			auto& confirm_folder = folder_dialog->create_child<button>();
			auto& confirm_folder_txt = confirm_folder.create_child<text>();
			confirm_folder_txt.set_text("confirm");
			confirm_folder.callback = [this]{
				confirm_folder_dialog();
			};
			{
				auto& spacer = folder_dialog->create_child<element>();
				X(spacer.min_size) = 5;
			}
			auto& cancel_folder = folder_dialog->create_child<button>();
			auto& cancel_folder_txt = cancel_folder.create_child<text>();
			cancel_folder_txt.set_text("cancel");
			cancel_folder.callback = [this]{
				toggle_add_folder_dialog(false);
			};
		}
	}
	{
		auto& border = create_child<panel>();
		border.min_size = {0,1};
		border.expand[layout::horizontal] = true;
	}

	view = &create_child<scroll_view>();
	view->expand = {true, true};
	view->content.create_layout<gui::layout::box>();
	view->content.expand[layout::horizontal] = true;

	{
		auto& border = create_child<panel>();
		border.min_size = {0,1};
		border.expand[layout::horizontal] = true;
	}

	auto& horizontal_container = create_child<element>();
	horizontal_container.create_layout<gui::layout::box>().orient = layout::horizontal;
	horizontal_container.expand[layout::horizontal] = true;

	{
		auto& spacer = horizontal_container.create_child<element>();
		spacer.expand[layout::horizontal] = true;
	}
	{
		auto& cancel = horizontal_container.create_child<button>();
		auto& txt = cancel.create_child<text>();
		txt.set_text("cancel");
		cancel.callback = [this]{
			set_visible(false);
		};
	}
	{
		auto& spacer = horizontal_container.create_child<element>();
		X(spacer.min_size) = 5;
	}
	{
		confirm = &horizontal_container.create_child<button>();
		auto& txt = confirm->create_child<text>();
		txt.set_text("confirm");
	}

	// events
	context->input_manager.key_press.subscribe(filename_field, [this](int key, int mods) {
		filename_field->on_key_press(key, mods);//TODO need a way to additively subscribe to events

		if(key == GLFW_KEY_ENTER)
			confirm->callback();
	});
}

void file_dialog::set_visible(bool visible)
{
	if(visibility_element == nullptr)
		visibility_element = this;
	visibility_element->set_visible(visible);
}

button& file_dialog::create_path(fs::path const& path, std::string str)
{
	auto update_text = [&](text& txt) {
		if(str.empty())
			txt.set_text(path.filename().string());
		else
			txt.set_text(str);

		// distinguish file/dir by color
		if(fs::is_directory(path))
			txt.color = nvgRGBA(255, 255, 255, 255);
		else
			txt.color = nvgRGBA(180, 255, 180, 255);
	};

	button* btn;

	// reuse existing buttons if available
	if(paths_count < view->content.children.size()) {
		btn = static_cast<button*>(view->content.children.at(paths_count).get());
		auto& txt = static_cast<text&>(*btn->children.at(0));
		update_text(txt);
	}
	else {
		btn = &view->content.create_child<button>();
		btn->expand[layout::horizontal] = true;

		auto& txt = btn->create_child<text>();
		update_text(txt);
	}

	++paths_count;
	return *btn;
}

void file_dialog::add_path_pick(fs::path const& path, std::string str)
{
	auto& btn = create_path(path, str);

	// display directory's content when clicked
	if(fs::is_directory(path))
	{
		btn.callback = [this, path]{
			// callback may be overwitten from within itself, deleting path.
			auto const path_copy = path;
			pick_change_dir(path_copy);
		};
	}
	else {
		btn.callback = [this, path]{
			set_visible(false);
			current_callback(path);
		};
	}
}
void file_dialog::add_path_save(fs::path const& path, std::string str)
{
	auto& btn = create_path(path, str);

	// display directory's content when clicked
	if(fs::is_directory(path))
	{
		btn.callback = [this, path]{
			// callback may be overwitten from within itself, deleting path.
			auto const path_copy = path;
			save_change_dir(path_copy);
		};
	}
	// set filename to clicked file
	else {
		btn.callback = [this, path]{
			filename_field->set_text(path.filename().string());
		};
	}
}

void file_dialog::pick_file(fs::path dir, std::function<void(fs::path)> callback, std::vector<fs::path> const& extensions)
{
	mode = Mode::pick;
	current_callback = callback;
	current_extensions = extensions;
	current_dir = dir;

	auto canon = fs::canonical(dir);

	path_field->set_text(canon.string());

	context->input_manager.key_press.subscribe(path_field, [this](int key, int mods) {
		if(key == GLFW_KEY_ENTER && fs::exists(path_field->str))
			pick_change_dir(path_field->str);
	});

	title.set_text("pick file...");

	set_visible(true);
	filename_container.set_visible(false);
	confirm->set_visible(false);
	add_folder->set_visible(false);

	pick_change_dir(canon);
}

void file_dialog::pick_file(fs::path dir, std::function<void(fs::path)> callback, fs::path extension)
{
	auto const extentions = std::vector{extension};
	pick_file(dir, callback, extentions);
}


void file_dialog::pick_change_dir(fs::path const& dir)
{
	current_dir = dir;
	paths_count = 0;

	path_field->set_text(dir.string());

	// "go back" button
	if(dir.has_parent_path())
	{
		auto const parent = dir.parent_path();
		add_path_pick(parent, "..");
	}

	// create representations for paths in the viewed directory
	for(auto const& p : fs::directory_iterator(dir))
	{
		if(current_extensions.empty() || fs::is_directory(p.path())) {
			add_path_pick(p.path());
			continue;
		}

		auto const p_ext = p.path().extension();
		for(auto const& ext : current_extensions) {
			if(p_ext == ext) {
				add_path_pick(p.path());
				continue;
			}
		}
	}

	// delete unused buttons
	view->content.children.resize(paths_count);
}


void file_dialog::save_file(fs::path dir, std::function<void(fs::path)> callback, fs::path extension)
{
	mode = Mode::save;
	current_callback = callback;
	current_extension = extension;
	current_dir = dir;

	auto canon = fs::canonical(dir);

	path_field->set_text(canon.string());

	context->input_manager.key_press.subscribe(path_field, [this](int key, int mods) {
		path_field->on_key_press(key, mods);//TODO need a way to additively subscribe to events

		if(key == GLFW_KEY_ENTER && fs::exists(path_field->str))
			save_change_dir(path_field->str);
	});

	title.set_text("save file...");

	set_visible(true);
	filename_container.set_visible(true);
	confirm->set_visible(true);
	add_folder->set_visible(true);

	save_change_dir(canon);
}

void file_dialog::save_change_dir(fs::path const& dir)
{
	current_dir = dir;
	paths_count = 0;

	path_field->set_text(dir.string());

	// "go back" button
	if(dir.has_parent_path())
	{
		auto const parent = dir.parent_path();
		add_path_save(parent, "..");
	}

	// create representations for paths in the viewed directory
	for(auto const& p : fs::directory_iterator(dir))
	{
		if(current_extension.empty() || fs::is_directory(p.path()) || p.path().extension() == current_extension)
			add_path_save(p.path());
	}

	confirm->callback = [this]{
		// don't allow empty filename
		if(filename_field->str.empty()) {
			context->input_manager.set_focused_element(filename_field);
			return;
		}

		set_visible(false);

		fs::path filename = filename_field->str;
		if(filename.extension() != current_extension)
			filename += current_extension;

		current_callback(current_dir / filename);
	};

	// delete unused buttons
	view->content.children.resize(paths_count);
}

void file_dialog::toggle_add_folder_dialog(bool show)
{
	path_field->set_visible(!show);
	add_folder->set_visible(!show);
	folder_field->set_visible(show);
	folder_dialog->set_visible(show);

	folder_field->set_text("");
	context->input_manager.set_focused_element(folder_field);

	// make sure the file_dialog's width doesn't change
	if(show) {
		original_min_size = X(min_size);
		X(min_size) = X(size);
	}
	else {
		X(min_size) = original_min_size;
	}
}

void file_dialog::confirm_folder_dialog()
{
	auto new_dir = current_dir / folder_field->str;
	fs::create_directory(new_dir);

	// open the new directory
	if(mode == Mode::pick) {
		pick_change_dir(new_dir);
	}
	else if(mode == Mode::save) {
		save_change_dir(new_dir);
	}

	toggle_add_folder_dialog(false);
}

void file_dialog::refresh()
{
	if(mode == Mode::pick) {
		pick_change_dir(current_dir);
	}
	else if(mode == Mode::save) {
		save_change_dir(current_dir);
	}
}