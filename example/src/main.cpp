#include <framework/application.h>
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
	auto& vg = app.vg;


	auto font = nvgCreateFont(vg, "sans", "../core-gui/example/font/Roboto-Regular.ttf");
	if (font == -1) {
		printf("Could not add font.\n");
		return 1;
	}


	// Test alignment issues with text at the very top of the window
	text txt;
	txt.str = "TopText";
	txt.font = font;
	txt.font_size = 14;
	txt.color = nvgRGBA(255, 255, 255, 255);
	txt.update_bounds(vg);


	// editable text
	{
		auto& root = app.root.create_child<panel>();
		root.position = { 50,100 };
		root.color = nvgRGBA(100,100,100, 255);
		
		root.create_layout<gui::layout::box>();


		auto& txt_edit = root.create_child<text_edit>(vg, app.input_manager);
		txt_edit.min_size = { 200,50 };
		txt_edit.str = "editable text";
		txt_edit.font = font;
		txt_edit.font_size = 18;
		txt_edit.color = nvgRGBA(255, 255, 255, 255);
		txt_edit.update_bounds(vg);

		root.child_layout->perform();
	}


	// button test
	{
		auto& el = app.root.create_child<button>(app.input_manager);
		el.position = { 500,200 };
		el.background_color = el.color = nvgRGBA(80, 80, 80, 255);
		el.hover_color = nvgRGBA(120, 120, 120, 255);
		el.press_color = nvgRGBA(0, 120, 210, 255);

		el.label.str = "button";
		el.label.font = font;
		el.label.font_size = 18;
		el.label.update_bounds(vg);

		el.child_layout->perform();

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
			txt.str = "Testing";
			txt.font = font;
			txt.font_size = 50;
			txt.color = nvgRGBA(255, 255, 255, 255);
			txt.update_bounds(vg);
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
		txt.draw_recursive(vg);
		app.root.draw_recursive(vg);
	});
}