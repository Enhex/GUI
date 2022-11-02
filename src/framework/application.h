#pragma once

#include "../gui/gui.h"
#include "../include_glfw.h"
#include "serialization.h"

/*
for now should only create 1 instance since it initializes libraries.
*/
struct application : gui::context
{
	application(int width, int height, const char* title, GLFWmonitor* monitor = nullptr, GLFWwindow* share = nullptr);

	~application();

	GLFWwindow* window = nullptr;

	element root;//TODO hardcode the root element as a panel, and automatically size it to the screen so it acts as the background?

	template<typename F>
	void run(F loop_function);

	std::string load_deco_file(std::string const& filepath);

	void save_style_file(std::string const& filepath);
	void load_style_file(std::string const& filepath);

	void load_layout(std::string const& filepath, element& parent);

	// return true if the key is pressed
	bool key_pressed(int key) const;

protected:
	void initialize();
	void create_window(int width, int height, const char* title, GLFWmonitor* monitor, GLFWwindow* share);

	static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);
	static void character_callback(GLFWwindow* window, unsigned int codepoint);

	static void cursor_pos_callback(GLFWwindow* window, double xpos, double ypos);
	static void mouse_button_callback(GLFWwindow* window, int button, int action, int mods);
	static void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
	static void window_size_callback(GLFWwindow* window, int width, int height);

	element* find_hovered_element(element& el);
	void update_hoevered_element();
};


template<typename F>
inline void application::run(F loop_function)
{
	// main loop
	while (!glfwWindowShouldClose(window))
	{
		// get window size
		int win_width, win_height;
		glfwGetWindowSize(window, &win_width, &win_height);

		// calculate pixel ratio
		int fb_width, fb_height;
		glfwGetFramebufferSize(window, &fb_width, &fb_height);
		auto const pixel_ratio = (float)fb_width / (float)win_width;

		glViewport(0, 0, fb_width, fb_height);
		glClear(GL_COLOR_BUFFER_BIT);

		nvgBeginFrame(vg, (float)win_width, (float)win_height, pixel_ratio);

		update_hoevered_element();
		input_manager.frame_start.send_global_event();

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
