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
		auto& bg = create_child<panel>();
		bg.create_layout<gui::layout::box>();
		bg.expand[layout::horizontal] = true;
		bg.color = NVGcolor{0.15,0.15,0.15,1};

		path_field = &bg.create_child<text_edit>();
		path_field->expand[layout::horizontal] = true;
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
	visibility_element->visible = visible;
}

void file_dialog::pick_file(fs::path dir, std::function<void(fs::path)> callback, std::vector<fs::path> const& extensions)
{
	auto canon = fs::canonical(dir);

	path_field->set_text(canon.string());

	context->input_manager.key_press.subscribe(path_field, [this, callback, extensions](int key, int mods) {
		path_field->on_key_press(key, mods);//TODO need a way to additively subscribe to events

		if(key == GLFW_KEY_ENTER && fs::exists(path_field->str))
			pick_file(path_field->str, callback, extensions);
	});

	title.set_text("choose file...");

	set_visible(true);
	filename_container.visible = false;
	confirm->visible = false;

	auto add_path = [&](fs::path const& path, std::string str = "")
	{
		auto& btn = view->content.create_child<button>();
		btn.expand[layout::horizontal] = true;

		auto& txt = btn.create_child<text>();
		if(str.empty())
			txt.set_text(path.filename().string());
		else
			txt.set_text(str);

		// display directory's content when clicked
		if(fs::is_directory(path))
		{
			btn.callback = [this, path, callback, extensions]{
				pick_file(path, callback, extensions);
			};
		}
		else {
			btn.callback = [this, path, callback]{
				set_visible(false);
				callback(path);
			};
		}
	};

	view->content.children.clear();

	// "go back" button
	if(canon.has_parent_path())
	{
		auto const parent = canon / "..";
		add_path(parent, "..");
	}

	// create representations for paths in the viewed directory
	for(auto const& p : fs::directory_iterator(dir))
	{
		if(extensions.empty() || fs::is_directory(p.path())) {
			add_path(p.path());
			continue;
		}

		auto const p_ext = p.path().extension();
		for(auto const& ext : extensions) {
			if(p_ext == ext) {
				add_path(p.path());
				continue;
			}
		}
	}
}

void file_dialog::pick_file(std::filesystem::path dir, std::function<void(std::filesystem::path)> callback, std::filesystem::path extension)
{
	pick_file(dir, callback, std::vector{extension});
}


void file_dialog::save_file(fs::path dir, std::function<void(fs::path)> callback, fs::path extension)
{
	auto canon = fs::canonical(dir);

	path_field->set_text(canon.string());

	context->input_manager.key_press.subscribe(path_field, [this, callback, extension](int key, int mods) {
		path_field->on_key_press(key, mods);//TODO need a way to additively subscribe to events

		if(key == GLFW_KEY_ENTER && fs::exists(path_field->str))
			save_file(path_field->str, callback, extension);
	});

	title.set_text("save file...");

	set_visible(true);
	filename_container.visible = true;
	confirm->visible = true;

	auto add_path = [&](fs::path const& path, std::string str = "")
	{
		auto& btn = view->content.create_child<button>();
		btn.expand[layout::horizontal] = true;

		auto& txt = btn.create_child<text>();
		if(str.empty())
			txt.set_text(path.filename().string());
		else
			txt.set_text(str);

		// display directory's content when clicked
		if(fs::is_directory(path))
		{
			btn.callback = [this, path, callback, extension]{
				save_file(path, callback, extension);
			};
		}
		// set filename to clicked file
		else {
			btn.callback = [this, path, callback]{
				filename_field->set_text(path.filename().string());
			};
		}
	};

	view->content.children.clear();

	// "go back" button
	if(canon.has_parent_path())
	{
		auto const parent = canon / "..";
		add_path(parent, "..");
	}

	// create representations for paths in the viewed directory
	for(auto const& p : fs::directory_iterator(dir))
	{
		if(extension.empty() || fs::is_directory(p.path()) || p.path().extension() == extension)
			add_path(p.path());
	}

	confirm->callback = [this, canon, callback, extension]{
		set_visible(false);

		fs::path filename = filename_field->str;
		if(filename.extension() != extension)
			filename += extension;

		callback(canon / filename);
	};
}