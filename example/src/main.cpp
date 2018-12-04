#include <deco/all.h>
#include <framework/application.h>
#include <fstream>
#include <gui/gui.h>
#include <iostream>

constexpr int window_width = 640;
constexpr int window_height = 480;

auto rand_bright() {
	return rand() % 128 + 127;
};

auto random_color() {
	return nvgRGBA(rand_bright(), rand_bright(), rand_bright(), 255);
}

int main()
{
	application app(window_width, window_height, "GUI example", nullptr, nullptr);
	element::context = &app;

	auto& vg = app.vg;


	auto font = nvgCreateFont(vg, "sans", "../GUI/example/font/Roboto-Regular.ttf"); // in case of running from build folder
	if (font == -1) {
		font = nvgCreateFont(vg, "sans", "../../../GUI/example/font/Roboto-Regular.ttf"); // in case of running from bin folder
		if (font == -1) {
			printf("Could not add font.\n");
			return 1;
		}
	}


	{
		auto& text_style = app.style_manager.styles["text"];
		text_style.emplace(std::make_pair("font", font));
		text_style.emplace(std::make_pair("font_size", 18.f));
		text_style.emplace(std::make_pair("color", nvgRGBA(255, 255, 255, 255)));
	}


	// Test using style file
	{
		deco::OutputStream_indent stream;

		for (auto&[name, properties] : app.style_manager.styles) {
			deco::serialize(stream, deco::make_list(name, properties));
		}

		std::ofstream file("style.deco", std::ios::binary);
		file << stream.str;
	}

	app.style_manager.styles.clear();

	{
		auto file = std::ifstream("style.deco", std::ios::binary);
		std::string file_str{
			std::istreambuf_iterator<char>(file),
			std::istreambuf_iterator<char>() };

		auto stream = deco::make_InputStream(file_str.cbegin());

		while (stream.position != file_str.cend() && !stream.peek_list_end())
		{
			std::string name;
			style::style_st properties;
			deco::serialize(stream, deco::begin_list(name));
			deco::serialize(stream, properties);
			deco::serialize(stream, deco::end_list);

			app.style_manager.styles.emplace(name, properties);
		}
	}


	// test using layout file

	// write to file
	{
		deco::OutputStream_indent stream;

		panel el;
		el.position = { 50,50 };
		el.min_size = { 20,25 };
		el.size = { 20,25 };
		el.color = { 0, 1, 0, 1 };

		deco::serialize(stream, deco::make_list("panel", el));

		text el2;
		el2.str = "hi";
		el2.position = { 55,50 };
		el2.color = { 1, 0, 0, 1 };

		deco::serialize(stream, deco::make_list("text", el2));

		auto file = std::ofstream("layout.deco", std::ios::binary);
		file << stream.str;
	}
	// read from file
	{
		auto file = std::ifstream("layout.deco", std::ios::binary);
		std::string file_str{
			std::istreambuf_iterator<char>(file),
			std::istreambuf_iterator<char>() };

		/*
		use pre-parsed Deco objects approach to simplify serialization
		specifically polymorphic serialization
		*/
		auto entries = deco::parse(file_str.begin(), file_str.end());
		
		for (auto& entry : entries)
		{
			using namespace deco;

			auto const& name = entry.content;

			if (name == "element") {
				auto& child = emplace_back_derived<element>(app.root.children);
				read(entry, child);
			}
			else if (name == "panel") {
				auto& child = emplace_back_derived<panel>(app.root.children);
				read(entry, child);
			}
			else if (name == "text") {
				auto& child = emplace_back_derived<text>(app.root.children);
				read(entry, child);
			}
		}
	}

	// Test alignment issues with text at the very top of the window
	text txt;
	txt.str = "TopText";
	txt.font = font;
	txt.font_size = 14;
	txt.color = nvgRGBA(255, 255, 255, 255);
	txt.update_bounds();


	// editable text
	{
		auto& background = app.root.create_child<panel>();
		background.position = { 50,100 };
		background.color = nvgRGBA(100,100,100, 255);
		background.min_size = { 200,0 };
		background.create_layout<gui::layout::box>();

		auto& txt_edit = background.create_child<text_edit>();
		txt_edit.str = "editable text";

		background.child_layout->perform();
	}


	// button test
	{
		auto& el = app.root.create_child<button>();
		el.position = { 500,200 };
		el.background_color = nvgRGBA(80, 80, 80, 255);
		el.hover_color = nvgRGBA(120, 120, 120, 255);
		el.press_color = nvgRGBA(0, 120, 210, 255);
		el.color = el.background_color;

		el.label.str = "button";

		el.child_layout->perform();
		el.size = { 100,50 };

		el.callback = []() { std::cout << "button clicked\n"; };
	}


	// extended element fit around children
	{
		auto& root = app.root.create_child<panel>();
		root.position = { 50, 200 };
		root.min_size = { 100,100 };
		root.color = random_color();

		root.create_layout<gui::layout::box>();

		auto& expander = root.create_child<panel>();
		expander.min_size = { 60,0 };
		expander.expand = {false, true};
		expander.color = random_color();

		expander.create_layout<gui::layout::box>();

		{
			auto& child = expander.create_child<panel>();
			child.min_size = { 50,150 };
			child.color = random_color();
		}
		{
			auto& child = expander.create_child<panel>();
			child.min_size = { 50,50 };
			child.color = random_color();
		}

		root.child_layout->perform();
	}


	// element tree test
	auto& root = app.root.create_child<panel>();
	root.position = { 200,50 };
	root.min_size = { 200,400 };
	root.color = random_color();

	auto& box = root.create_layout<gui::layout::box>();

	{
		auto& el = root.create_child<panel>();
		el.min_size = { 100,0 };
		el.expand[box.orient] = true;
		el.color = random_color();
	}
	{
		auto& el = root.create_child<panel>();
		el.create_layout<gui::layout::box>();
		el.min_size = { 0,100 };
		el.expand[layout::horizontal] = true;
		el.color = random_color();

		auto& parent = el;
		{
			auto& el = parent.create_child<panel>();
			el.create_layout<gui::layout::box>();
			el.color = nvgRGBA(0,0,0, 100);

			auto& txt = el.create_child<text>();
			txt.setup(font, 50, "Testing");
		}
	}
	{
		auto& el = root.create_child<panel>();
		el.expand = { true, true };
		el.color = random_color();
	}
	{
		auto& el = root.create_child<panel>();
		el.min_size = { 150,50 };
		el.color = random_color();
	}

	root.child_layout->perform();


	// test global event
	app.input_manager.subscribe_global<input::event::key_press>([](std::any&& args) {
		auto&[key, mods] = std::any_cast<input::event::key_press::params&>(args);
		std::cout << "(global) key pressed: " << key << "\n";
	});
	app.input_manager.subscribe_global<input::event::key_press>([&root](std::any&& args) {
		auto&[key, mods] = std::any_cast<input::event::key_press::params&>(args);
		if (key != 67)
			return;

		root.children.clear();

		std::cout << "cleared!\n";
	});

	// test focused hover events
	app.input_manager.subscribe<input::event::hover_start>(root.children[0].get(), [el = static_cast<panel*>(root.children[0].get())](std::any&& args) {
		el->color = nvgRGBA(255,0,0,255);
		std::cout << "hover start\n";
	});
	app.input_manager.subscribe<input::event::hover_end>(root.children[0].get(), [el = static_cast<panel*>(root.children[0].get())](std::any&& args) {
		el->color = nvgRGBA(255, 255, 255, 255);
		std::cout << "hover end\n";
	});
	app.input_manager.subscribe<input::event::mouse_press>(root.children[0].get(), [el = static_cast<panel*>(root.children[0].get())](std::any&& args) {
		auto&[button, mods] = std::any_cast<input::event::mouse_press::params&>(args);
		el->color = nvgRGBA(0, 255, 0, 255);
		std::cout << "mouse press: " << button << "\n";
	});

	// Loop until the user closes the window
	app.run([&]()
	{
		txt.draw_recursive();
		app.root.draw_recursive();
	});
}