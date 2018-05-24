#include "application.h"

#define NANOVG_GL2_IMPLEMENTATION
#include <nanovg_gl.h>

#include <stdexcept>
#include <tuple>

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
	//TODO
}

void application::cursor_pos_callback(GLFWwindow * window, double xpos, double ypos)
{
	auto& app = *static_cast<application*>(glfwGetWindowUserPointer(window));
	app.mouse_pos = { (float)xpos, (float)ypos };

	/*TODO
	- hover event
		- start with brute force search
		- hover start:
		hovered element isn't the same as the last hovered element
		- hover end:
		hovered element
	*/
	auto& input_manager = app.input_manager;

	auto find_hoevered_element = [&](element* parent)
	{
		auto recurse_impl = [&](element* parent, auto& func) -> bool
		{
			for (auto const& child : parent->children) {
				if (func(child.get(), func))
					return true;
			}

			if (parent->is_inside(app.mouse_pos)) {
				input_manager.set_hovered_element(parent);
				return true;
			}

			return false;
		};

		return recurse_impl(parent, recurse_impl);
	};

	if(!find_hoevered_element(app.root))
		input_manager.set_hovered_element(nullptr);

	//for (auto const& child : app.root->children) {
	//	// check if element subscribed to hover event
	//	auto iter = input_manager.focused_events.find(input_manager.hovered_element);
	//	if (iter != input_manager.focused_events.end()) {
	//		auto& subscribed_events = iter->second;
	//		auto event_iter = subscribed_events.find(input::event::hover_start::id);
	//		if (event_iter != subscribed_events.end()) {
	//			// check if cursor is inside the element rectangle
	//			if (child->is_inside(app.mouse_pos)) {
	//				// call callback
	//				event_iter->second(std::tuple<vector2 const&>{ app.mouse_pos });
	//			}
	//		}
	//	}
	//}
}

void application::mouse_button_callback(GLFWwindow * window, int button, int action, int mods)
{
	//TODO
}
