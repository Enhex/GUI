#include "../framework/application.h"
#include "../gui/gui.h"

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


	auto font = nvgCreateFont(vg, "sans", "../core-gui/font/Roboto-Regular.ttf");
	if (font == -1) {
		printf("Could not add font.\n");
		return 1;
	}


	panel el0;
	el0.position = { 200,50 };
	el0.size = { 200,400 };
	el0.color = nvgRGBA(rand_bright(), rand_bright(), rand_bright(), 255);

	gui::layout::box box;
	el0.layout = &box;
	box.parent = &el0;

	gui::layout::box box2;

	{
		auto& el = emplace_back_derived<panel>(el0.children);
		el.size = { 100,100 };
		el.expand[box.orient] = true;
		el.color = nvgRGBA(rand_bright(), rand_bright(), rand_bright(), 255);
	}
	{
		auto& el = emplace_back_derived<panel>(el0.children);
		el.size = { 100,100 };
		el.expand[layout::horizontal] = true;
		el.color = nvgRGBA(rand_bright(), rand_bright(), rand_bright(), 255);

		el.layout = &box2;
		box2.parent = &el;

		auto& txt = emplace_back_derived<text>(el.children);
		txt.str = "Testing";
		txt.font = font;
		txt.font_size = 50;
		txt.size = { 100,100 };//TODO testing, should automatically set minimum size according to text bounds
		txt.color = nvgRGBA(255, 255, 255, 255);
	}
	{
		auto& el = emplace_back_derived<panel>(el0.children);
		el.size = { 100,100 };
		el.expand[box.orient] = true;
		el.expand[layout::horizontal] = true;
		el.color = nvgRGBA(rand_bright(), rand_bright(), rand_bright(), 255);
	}
	{
		auto& el = emplace_back_derived<panel>(el0.children);
		el.size = { 150,50 };
		el.color = nvgRGBA(rand_bright(), rand_bright(), rand_bright(), 255);
	}

	box.layout();
	box2.layout();


	// Loop until the user closes the window
	app.run([&]()
	{
		el0.draw_recursive(vg);
	});
}