#include <framework/application.h>
#include <gui/gui.h>

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
		//el.min_size = { 100,100 };
		el.expand = { true, true };
		el.color = nvgRGBA(rand_bright(), rand_bright(), rand_bright(), 255);
	}
	{
		auto& el = emplace_back_derived<panel>(root.children);
		el.min_size = { 150,50 };
		el.color = nvgRGBA(rand_bright(), rand_bright(), rand_bright(), 255);
	}

	box();


	// Loop until the user closes the window
	app.run([&]()
	{
		root.draw_recursive(vg);
	});
}