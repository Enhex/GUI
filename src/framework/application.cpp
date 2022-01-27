#include "application.h"

#define NANOVG_GL2_IMPLEMENTATION
#include <nanovg_gl.h>

#include <fstream>
#include <stdexcept>

application::application(int width, int height, const char* title, GLFWmonitor* monitor, GLFWwindow* share)
{
	initialize();

	create_window(width, height, title, monitor, share);

	root.min_size = { (float)width, (float)height };

	vg = nvgCreateGL2(NVG_ANTIALIAS | NVG_STENCIL_STROKES | NVG_DEBUG);

	element::context = this;
}

application::~application()
{
	glfwTerminate();
}

std::string application::load_deco_file(std::string const & filepath)
{
	auto file = std::ifstream(filepath, std::ios::binary);
	std::string file_str{
		std::istreambuf_iterator<char>(file),
		std::istreambuf_iterator<char>() };

	if (file_str.back() != deco::entry_delimiter)
		file_str.push_back(deco::entry_delimiter);

	return file_str;
}

void application::save_style_file(std::string const & filepath)
{
	deco::OutputStream_indent stream;

	for (auto&[name, properties] : style_manager.styles) {
		deco::serialize(stream, deco::make_list(name, properties));
	}

	std::ofstream file(filepath, std::ios::binary);
	file << stream.str;
}

void application::load_style_file(std::string const & filepath)
{
	std::string file_str = load_deco_file(filepath);

	auto stream = deco::make_InputStream(file_str.cbegin());

	while (stream.position != file_str.cend() && !stream.peek_list_end())
	{
		std::string name;
		style::style_st properties;
		deco::serialize(stream, deco::begin_list(name));
		deco::serialize(stream, properties);
		deco::serialize(stream, deco::end_list);

		style_manager.styles.emplace(name, properties);
	}
}

void application::load_layout(std::string const & filepath, element& parent)
{
	std::string file_str = load_deco_file(filepath);

	/*
	use pre-parsed Deco objects approach to simplify serialization
	specifically polymorphic serialization
	*/
	auto entries = deco::parse(file_str.begin(), file_str.end());

	for (auto& entry : entries)
	{
		read_element(entry, parent);
	}
}

void application::initialize()
{
	if (!glfwInit())
		throw std::runtime_error("GLFW initialization failed.");
}

void application::create_window(int width, int height, const char * title, GLFWmonitor * monitor, GLFWwindow * share)
{
	// Create a windowed mode window and its OpenGL context
	window = glfwCreateWindow(width, height, title, NULL, NULL);
	if (!window)
		throw std::runtime_error("Window or OpenGL context creation failed.");

	// Make the window's context current
	glfwMakeContextCurrent(window);

	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
		throw std::runtime_error("GLAD loading failed.");

	glfwSetWindowUserPointer(window, this);

	glfwSetKeyCallback(window, key_callback);
	glfwSetCharCallback(window, character_callback);

	glfwSetCursorPosCallback(window, cursor_pos_callback);
	glfwSetMouseButtonCallback(window, mouse_button_callback);
	glfwSetScrollCallback(window, scroll_callback);
	glfwSetWindowSizeCallback(window, window_size_callback);
}

void application::key_callback(GLFWwindow * window, int key, int scancode, int action, int mods)
{
	auto& input_manager = static_cast<application*>(glfwGetWindowUserPointer(window))->input_manager;

	switch (action) {
	case GLFW_RELEASE:
		input_manager.key_release.send_event(input_manager.focused_element, key, mods);
		break;
	case GLFW_PRESS:
		input_manager.key_press.send_event(input_manager.focused_element, key, mods);
		break;
	case GLFW_REPEAT:
		input_manager.key_repeat.send_event(input_manager.focused_element, key, mods);
		break;
	}
}

void application::character_callback(GLFWwindow * window, unsigned int codepoint)
{
	auto& input_manager = static_cast<application*>(glfwGetWindowUserPointer(window))->input_manager;
	input_manager.character.send_event(input_manager.focused_element, codepoint);
}

void application::cursor_pos_callback(GLFWwindow * window, double xpos, double ypos)
{
	auto& app = *static_cast<application*>(glfwGetWindowUserPointer(window));
	app.input_manager.mouse_pos = { (float)xpos, (float)ypos };
}

element* application::find_hovered_element(element& el)
{
	/*
	recursively search for the leaf-most element that the mouse fits inside.
	also need to search child order back-to-front, because
	child rendering is front-to-back (so child that's rendered on top of others takes priority).

	TODO:
	- spatial partitioning to optimize search
	*/

	// don't consider invisible element
	if(!el.get_visible())
		return nullptr;

	// exclude cursor position which is outside the intersection of all visited scissors.
	if(auto* sci = dynamic_cast<scissor*>(&el)) {
		if(!sci->is_inside(input_manager.mouse_pos)) {
			return nullptr;
		}
	}

	// check if a descendant is hovered first
	for (auto child_iter = el.children.rbegin(); child_iter != el.children.rend(); ++child_iter) {
		auto hovered_descendant = find_hovered_element(**child_iter);
		if (hovered_descendant != nullptr)
			return hovered_descendant;
	}

	// check if current element is hovered
	if (el.is_inside(input_manager.mouse_pos)) {
		return &el;
	}

	return nullptr;
}

void application::update_hoevered_element()
{
	auto hovered_el = find_hovered_element(root);
	input_manager.set_hovered_element(hovered_el);
}

void application::mouse_button_callback(GLFWwindow * window, int button, int action, int mods)
{
	using namespace input;

	auto& input_manager = static_cast<application*>(glfwGetWindowUserPointer(window))->input_manager;

	// hovered element only gains focus on mouse press (not release)
	if (action == GLFW_PRESS && input_manager.hovered_element != nullptr) {
		//TODO allow controlling if an element auto gains focus on click
		input_manager.set_focused_element(input_manager.hovered_element);
		input_manager.pressed_element = input_manager.hovered_element;
	}

	if (action == GLFW_RELEASE)
		input_manager.mouse_release.send_event(input_manager.hovered_element, button, mods);
	if (action == GLFW_PRESS)
		input_manager.mouse_press.send_event(input_manager.hovered_element, button, mods);

	// reset pressed element after release event was sent, so the callback can know which element was pressed
	if (action == GLFW_RELEASE)
		input_manager.pressed_element = nullptr;
}

void application::scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
	auto& input_manager = static_cast<application*>(glfwGetWindowUserPointer(window))->input_manager;
	input_manager.scroll.send_event(input_manager.hovered_element, xoffset, yoffset);
}

void application::window_size_callback(GLFWwindow* window, int width, int height)
{
	// use a callback to avoid resizing every frame, which is useful if layout is only updated when needed.
	auto& root = static_cast<application*>(glfwGetWindowUserPointer(window))->root;
	root.size = root.min_size = {(float)width, (float)height};
}