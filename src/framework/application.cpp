#include "application.h"

#define NANOVG_GL2_IMPLEMENTATION
#include <nanovg_gl.h>

#include <fstream>
#include <stdexcept>
#include <tuple>

application::application(int width, int height, const char* title, GLFWmonitor* monitor, GLFWwindow* share)
{
	initialize();

	create_window(width, height, title, monitor, share);

	root.min_size = { (float)width, (float)height };

	vg = nvgCreateGL2(NVG_ANTIALIAS | NVG_STENCIL_STROKES | NVG_DEBUG);
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
		throw std::runtime_error("GLAD loading failed.");

	// Make the window's context current
	glfwMakeContextCurrent(window);

	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
		throw std::runtime_error("GLAD loading failed.");

	glfwSetWindowUserPointer(window, this);

	glfwSetKeyCallback(window, key_callback);
	glfwSetCharCallback(window, character_callback);

	glfwSetCursorPosCallback(window, cursor_pos_callback);
	glfwSetMouseButtonCallback(window, mouse_button_callback);
}

void application::key_callback(GLFWwindow * window, int key, int scancode, int action, int mods)
{
	auto& input_manager = static_cast<application*>(glfwGetWindowUserPointer(window))->input_manager;
	
	auto const event_id = [&]() {
		if (action == GLFW_RELEASE)
			return input::event::key_release::id;
		if (action == GLFW_PRESS)
			return input::event::key_press::id;
		if (action == GLFW_REPEAT)
			return input::event::key_repeat::id;
		//throw std::invalid_argument("unknown action");
		return input::event::invalid_id;
	}();

	if (input_manager.focused_element != nullptr) {
		input_manager.send_focused_event(input_manager.focused_element, event_id, std::tuple{ key, mods });
	}
	else {
		input_manager.send_global_event(event_id, std::tuple{ key, mods });
	}
}

void application::character_callback(GLFWwindow * window, unsigned int codepoint)
{
	auto& input_manager = static_cast<application*>(glfwGetWindowUserPointer(window))->input_manager;

	if (input_manager.focused_element != nullptr) {
		input_manager.send_focused_event(input_manager.focused_element, input::event::character::id, std::tuple{ codepoint });
	}
	else {
		input_manager.send_global_event(input::event::character::id, std::tuple { codepoint });
	}
}

void application::cursor_pos_callback(GLFWwindow * window, double xpos, double ypos)
{
	auto& app = *static_cast<application*>(glfwGetWindowUserPointer(window));
	app.input_manager.mouse_pos = { (float)xpos, (float)ypos };

	/*TODO
	- hover event
		- start with brute force search
		- hover start:
		hovered element isn't the same as the last hovered element
		- hover end:
		hovered element
	*/
	auto& input_manager = app.input_manager;

	// recursively search for the leaf-most element that the mouse fits inside
	auto find_hoevered_element = [&](element* parent)
	{
		auto recurse_impl = [&](element* el, auto& func) -> bool
		{
			for (auto const& child : el->children) {
				if (func(child.get(), func))
					return true;
			}

			auto const& focused_events = input_manager.get_focused_events();
			if (focused_events.find(el) != focused_events.end() &&	// if the element doesn't do something with the event, propagate it to the parent.
				el->is_inside(app.input_manager.mouse_pos)) {
				input_manager.set_hovered_element(el);
				return true;
			}

			return false;
		};

		return recurse_impl(parent, recurse_impl);
	};

	if(!find_hoevered_element(&app.root))
		input_manager.set_hovered_element(nullptr);
}

void application::mouse_button_callback(GLFWwindow * window, int button, int action, int mods)
{
	using namespace input;

	auto const event_id = [&]() {
		if (action == GLFW_RELEASE)
			return event::mouse_release::id;
		if (action == GLFW_PRESS)
			return event::mouse_press::id;
		return event::invalid_id;
	}();


	auto& input_manager = static_cast<application*>(glfwGetWindowUserPointer(window))->input_manager;


	if (input_manager.hovered_element != nullptr) {
		//TODO allow controlling if an element auto gains focus on click
		input_manager.focused_element = input_manager.hovered_element;

		if(event_id == event::mouse_press::id)
			input_manager.pressed_element = input_manager.hovered_element;

		input_manager.send_focused_event(input_manager.hovered_element, event_id, std::tuple{ button, mods });
	}
	else {
		input_manager.send_global_event(event_id, std::tuple{ button, mods });
	}

	// reset pressed element after release event was sent, so the callback can know which element was pressed
	if (event_id == event::mouse_release::id)
		input_manager.pressed_element = nullptr;
}
