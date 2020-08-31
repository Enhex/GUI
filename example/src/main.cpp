#include <deco/all.h>
#include <framework/application.h>
#include <fstream>
#include <gui/gui.h>
#include <iostream>

constexpr int window_width = 800;
constexpr int window_height = 800;

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


	auto font = app.style_manager.create_font(vg, "../../GUI/example/font/Roboto-Regular.ttf"); // in case of running from build folder
	if (font == -1) {
		font = app.style_manager.create_font(vg, "../../../../GUI/example/font/Roboto-Regular.ttf"); // in case of running from bin folder
		if (font == -1) {
			printf("Could not add font.\n");
			return 1;
		}
	}


	{
		auto const& font_name = app.style_manager.font_id_to_name(font);

		auto& style = app.style_manager.styles["text"];
		style.emplace(std::make_pair("font", font_name));
		style.emplace(std::make_pair("font_size", 18.f));
		style.emplace(std::make_pair("color", nvgRGBA(255, 255, 255, 255)));
	}
	{
		auto& style = app.style_manager.styles["green"];
		style.emplace(std::make_pair("color", nvgRGBA(0, 255, 0, 255)));
	}
	{
		auto& style = app.style_manager.styles["red"];
		style.emplace(std::make_pair("color", nvgRGBA(255, 0, 0, 255)));
	}


	// Test using style file
	app.save_style_file("style.deco");
	app.style_manager.styles.clear();
	app.load_style_file("style.deco");


	// test using layout file

	// write to file
	{
		deco::OutputStream_indent stream;

		panel el;
		el.set_id("first");
		el.position = { 50,50 };
		el.min_size = { 20,25 };
		el.style = "green";
		el.expand = { 1,0 }; // doesnt affect anything currently, just testing serialization
		el.create_layout<gui::layout::box>().orient = layout::horizontal;

		auto& scissor_child = el.create_child<scissor>();
		scissor_child.expand = { true, true };

		auto& child = scissor_child.create_child<text>();
		child.str = "I'm a child element";
		child.style = "red";

		deco::serialize(stream, deco::make_list(el.get_element_name(), el));

		text_edit el2;
		el2.str = "hi";
		el2.position = { 55,30 };

		deco::serialize(stream, deco::make_list(el2.get_element_name(), el2));

		auto file = std::ofstream("layout.deco", std::ios::binary);
		file << stream.str;
	}
	// read from file
	app.load_layout("layout.deco", app.root);

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

	// nested box sizing test
	{
		auto& root = app.root.create_child<panel>();
		root.position = { 500,300 };
		root.min_size = { 250,250 };
		root.color = nvgRGBA(0, 0, 255, 255);
		auto& box = root.create_layout<gui::layout::box>();

		auto& el = root.create_child<panel>();
		el.expand = {true, true};
		el.color = random_color();
		el.create_layout<gui::layout::box>();

		auto& el2 = el.create_child<panel>();
		el2.min_size = { 20, 20 };
		el2.expand = {false, true};
		el2.color = random_color();

		root.child_layout->perform();
	}

	// scroll view test
	{
		auto& root = app.root.create_child<panel>();
		root.position = { 500,300 };
		root.min_size = { 250,250 };
		root.color = nvgRGBA(0, 0, 255, 255);
		auto& box = root.create_layout<gui::layout::box>();

		auto& sv = root.create_child<scroll_view>();
		sv.expand = {true, true};

		auto& p1 = sv.content.create_child<panel>();
		p1.position = { 50, 50 };
		p1.min_size = { 50, 450 };
		p1.color = random_color();
		auto& t1 = sv.content.create_child<text>();
		t1.setup(font, 25, "scroll view");
		t1.position = { 60, 60 };
		t1.color = nvgRGBA(255, 0, 0, 255);;

		root.child_layout->perform();
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
		//expander.expand = {false, true}; // expanding causes it to not fit around children, which kinda makes sense? be limited to the expansion size when expending? makes sense with expanding scissor view.
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
	root.color = nvgRGBA(127, 127, 127, 255);

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

	auto& toggle_el = root.create_child<panel>();
	toggle_el.expand = { true, true };
	toggle_el.color = random_color();

	{
		auto& el = root.create_child<panel>();
		el.min_size = { 150,50 };
		el.color = random_color();
	}

	root.child_layout->perform();


	// button test
	{
		auto& el = app.root.create_child<button>();
		el.position = { 500,200 };
		el.background_color = nvgRGBA(80, 80, 80, 255);
		el.hover_color = nvgRGBA(120, 120, 120, 255);
		el.press_color = nvgRGBA(0, 120, 210, 255);
		el.color = el.background_color;

		auto& label = el.create_child<text>();
		label.str = "button";

		el.child_layout->perform();
		el.size = { 100,50 };

		el.callback = [&]() {
			std::cout << "button clicked\n";
			toggle_el.visible = !toggle_el.visible;
			root.child_layout->perform();
		};
	}


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