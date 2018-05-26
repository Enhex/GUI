#include <framework/application.h>
#include <gui/gui.h>
#include <iostream>

// return reference to the original type instead of the base class
template <typename T, typename Container>
T& emplace_back_derived(Container& container)
{
	return static_cast<T&>(*container.emplace_back(std::make_unique<T>()).get());
}

constexpr int window_width = 640;
constexpr int window_height = 480;

auto rand_bright() {
	return rand() % 128 + 127;
};

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


	panel root;
	root.position = { 200,50 };
	root.min_size = { 200,400 };
	root.color = nvgRGBA(rand_bright(), rand_bright(), rand_bright(), 255);

	auto& box = root.create_layout<gui::layout::box>();

	{
		auto& el = emplace_back_derived<panel>(root.children);
		el.min_size = { 100,0 };
		el.expand[box.orient] = true;
		el.color = nvgRGBA(rand_bright(), rand_bright(), rand_bright(), 255);
	}
	{
		auto& el = emplace_back_derived<panel>(root.children);
		el.create_layout<gui::layout::box>();
		el.min_size = { 0,100 };
		el.expand[layout::horizontal] = true;
		el.color = nvgRGBA(rand_bright(), rand_bright(), rand_bright(), 255);

		auto& parent = el;
		{
			auto& el = emplace_back_derived<panel>(parent.children);
			el.create_layout<gui::layout::box>();
			el.color = nvgRGBA(0,0,0, 100);

			auto& txt = emplace_back_derived<text>(el.children);
			txt.str = "Testing";
			txt.font = font;
			txt.font_size = 50;
			txt.color = nvgRGBA(255, 255, 255, 255);
			txt.update_bounds(vg);
		}
	}
	{
		auto& el = emplace_back_derived<panel>(root.children);
		el.expand = { true, true };
		el.color = nvgRGBA(rand_bright(), rand_bright(), rand_bright(), 255);
	}
	{
		auto& el = emplace_back_derived<panel>(root.children);
		el.min_size = { 150,50 };
		el.color = nvgRGBA(rand_bright(), rand_bright(), rand_bright(), 255);
	}


	root.child_layout->fit();
	(*root.child_layout)();


	app.root = &root;

	// test global event
	app.input_manager.subscribe_global<input::event::key_press>(&root, [](std::any&& args) {
		auto&[key, mods] = std::any_cast<input::event::key_press::params&>(args);
		std::cout << "key pressed: " << key << "\n";
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
		root.draw_recursive(vg);
	});
}