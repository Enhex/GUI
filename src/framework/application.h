#pragma once

#include <glad/glad.h> // must be included before GLFW
#include <GLFW/glfw3.h>

#include "../gui/gui.h"

/*
for now should only create 1 instance since it initializes libraries.
*/
struct application
{
	application(int width, int height, const char* title, GLFWmonitor* monitor = nullptr, GLFWwindow* share = nullptr);

	~application();

	GLFWwindow* window = nullptr;
	NVGcontext* vg = nullptr;

	element root;//TODO hardcode the root element as a panel, and automatically size it to the screen so it acts as the background?

	input::manager input_manager;

	template<typename F>
	void run(F loop_function);

protected:
	void initialize();
	void create_window(int width, int height, const char* title, GLFWmonitor* monitor, GLFWwindow* share);

	static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);
	static void character_callback(GLFWwindow* window, unsigned int codepoint);

	static void cursor_pos_callback(GLFWwindow* window, double xpos, double ypos);
	static void mouse_button_callback(GLFWwindow* window, int button, int action, int mods);
};


template<typename F>
inline void application::run(F loop_function)
{
	// get window size
	int win_width, win_height;
	glfwGetWindowSize(window, &win_width, &win_height);

	// calculate pixel ratio
	int fb_width, fb_height;
	glfwGetFramebufferSize(window, &fb_width, &fb_height);
	auto const pixel_ratio = (float)fb_width / (float)win_width;

	// main loop
	while (!glfwWindowShouldClose(window))
	{
		glClear(GL_COLOR_BUFFER_BIT);

		nvgBeginFrame(vg, (float)win_width, (float)win_height, pixel_ratio);

		// user defined function
		loop_function();

		nvgEndFrame(vg);

		// Swap front and back buffers
		glfwSwapBuffers(window);

		// Poll for and process events
		glfwPollEvents();
		//TODO use glfwWaitEventsTimeout(framerate); instead?
	}
}
