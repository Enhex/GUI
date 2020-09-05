#include "file_dialog.h"

#include "button.h"
#include "scroll_view.h"
#include "text.h"

file_dialog::file_dialog() :
title(create_child<text>())
{
	color = NVGcolor{0.2,0.2,0.2,1};

	style = element_name;
	apply_style();
	
	visible = false;
	create_layout<gui::layout::box>();

	// elements
	{
		auto& border = create_child<panel>();
		border.min_size = {0,1};
		border.expand[layout::horizontal] = true;
	}

	view = &create_child<scroll_view>();
	view->expand = {true, true};
	view->content.create_layout<gui::layout::box>();

	{
		auto& border = create_child<panel>();
		border.min_size = {0,1};
		border.expand[layout::horizontal] = true;
	}
	{
		auto& cancel = create_child<button>();
		auto& txt = cancel.create_child<text>();
		txt.set_text("cancel");
		cancel.callback = [this]{
			visible = false;
		};
	}
}

void file_dialog::pick_file(std::filesystem::path dir, std::function<void(std::filesystem::path)> callback)
{
	title.set_text("choose file...");

	visible = true;

	auto add_path = [&](std::filesystem::path const& path, std::string str = "")
	{
		auto& btn = view->content.create_child<button>();
		auto& txt = btn.create_child<text>();
		if(str.empty())
			txt.set_text(path.filename().string());
		else
			txt.set_text(str);

		// display directory's content when clicked
		if(std::filesystem::is_directory(path))
		{
			btn.callback = [this, path, callback]{
				pick_file(path, callback);
			};
		}
		else {
			btn.callback = [this, path, callback]{
				visible = false;
				callback(path);
			};
		}
	};

	view->content.children.clear();

	// "go back" button
	auto abs = std::filesystem::absolute(dir);
	if(abs.has_parent_path())
	{
		auto const parent = abs / "..";
		add_path(parent, "..");
	}

	// create representations for paths in the viewed directory
	for(auto const& p : std::filesystem::directory_iterator(dir))
	{
		add_path(p.path());
	}
}