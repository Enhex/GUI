#include "../layout/layout.h"
#include "../framework/application.h"

constexpr int window_width = 640;
constexpr int window_height = 480;

auto rand_bright() {
	return rand() % 128 + 127;
};

int main(void)
{
	application app(window_width, window_height, "Hello World", nullptr, nullptr);
	auto& vg = app.vg;


	auto draw_rect = [&](rectangle const& rect) {
		nvgBeginPath(vg);
		nvgRect(vg, X(rect.position), Y(rect.position), X(rect.size), Y(rect.size));
		nvgFillColor(vg, nvgRGBA(rand_bright(), rand_bright(), rand_bright(), 255));
		nvgFill(vg);
	};

	element el0;
	el0.position = { 200,50 };
	el0.size = {200,400};
	layout::box box;

	el0.layout = &box;
	box.parent = &el0;

	{
		auto& el = el0.children.emplace_back();
		el.size = { 100,100 };
		el.expand[box.orient] = true;
	}
	{
		auto& el = el0.children.emplace_back();
		el.size = { 100,100 };
		el.expand[layout::horizontal] = true;
	}
	{
		auto& el = el0.children.emplace_back();
		el.size = { 100,100 };
		el.expand[box.orient] = true;
		el.expand[layout::horizontal] = true;
	}
	{
		auto& el = el0.children.emplace_back();
		el.size = { 150,50 };
	}

	box.layout();

	// Loop until the user closes the window
	app.run([&]()
	{
		srand(0);

		draw_rect(el0);
		for (auto const& child : el0.children) {
			draw_rect(child);
		}
	});
}