#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <nanovg.h>
#define NANOVG_GL2_IMPLEMENTATION
#include "nanovg_gl.h"

#include "../layout/layout.h"

constexpr int window_width = 640;
constexpr int window_height = 480;

int main(void)
{
	GLFWwindow* window;

	/* Initialize the library */
	if (!glfwInit())
		return -1;

	/* Create a windowed mode window and its OpenGL context */
	window = glfwCreateWindow(window_width, window_height, "Hello World", NULL, NULL);
	if (!window)
	{
		glfwTerminate();
		return -1;
	}

	/* Make the window's context current */
	glfwMakeContextCurrent(window);

	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
		return -1;

	auto vg = nvgCreateGL2(NVG_ANTIALIAS | NVG_STENCIL_STROKES | NVG_DEBUG);


	int fb_width, fb_height;
	glfwGetFramebufferSize(window, &fb_width, &fb_height);
	auto const pixel_ratio = (float)fb_width / (float)window_width;




	auto rand_bright = []() {
		return rand() % 128 +127;
	};

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

	/* Loop until the user closes the window */
	while (!glfwWindowShouldClose(window))
	{
		/* Render here */
		glClear(GL_COLOR_BUFFER_BIT);


		nvgBeginFrame(vg, window_width, window_height, pixel_ratio);


		srand(0);

		draw_rect(el0);
		for (auto const& child : el0.children) {
			draw_rect(child);
		}

		nvgEndFrame(vg);


		/* Swap front and back buffers */
		glfwSwapBuffers(window);

		/* Poll for and process events */
		glfwPollEvents();
	}

	glfwTerminate();
	return 0;
}