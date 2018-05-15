#include "application.h"

#define NANOVG_GL2_IMPLEMENTATION
#include <nanovg_gl.h>

#include <stdexcept>

application::application(int width, int height, const char* title, GLFWmonitor* monitor, GLFWwindow* share)
{
	initialize();

	create_window(width, height, title, monitor, share);

	vg = nvgCreateGL2(NVG_ANTIALIAS | NVG_STENCIL_STROKES | NVG_DEBUG);
}

application::~application()
{
	glfwTerminate();
}

void application::create_window(int width, int height, const char * title, GLFWmonitor * monitor, GLFWwindow * share)
{
	// Create a windowed mode window and its OpenGL context
	window = glfwCreateWindow(width, height, "Hello World", NULL, NULL);
	if (!window)
		throw std::runtime_error("GLAD loading failed.");

	// Make the window's context current
	glfwMakeContextCurrent(window);

	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
		throw std::runtime_error("GLAD loading failed.");
}

void application::initialize()
{
	if (!glfwInit())
		throw std::runtime_error("GLFW initialization failed.");
}