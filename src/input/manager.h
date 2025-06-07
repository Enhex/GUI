#pragma once

#include "../math/Vector2.h"
#include "element.h"
#include "events.h"

namespace gui::input
{
	struct element;

	/*TODO
	- global events context that the user can switch between
	*/
	struct manager
	{
		nx::Vector2 mouse_pos;
		bool cursor_in_window;

		element* focused_element = nullptr; // keyboard focus
		element* hovered_element = nullptr; // mouse focus
		element* pressed_element = nullptr; // element which was hovered when mouse press happened

		// events
		event::key_press key_press;
		event::key_release key_release;
		event::key_repeat key_repeat;
		event::hover_start hover_start;
		event::hover_end hover_end;
		event::mouse_press mouse_press;
		event::mouse_release mouse_release;
		event::double_click double_click;
		event::scroll scroll;
		event::character character;
		event::window_size window_size;
		event::frame_start frame_start;
		event::focus_start focus_start;
		event::focus_end focus_end;
		event::visible_start visible_start;
		event::visible_end visible_end;

		// to be implemented without inheritance
		std::function<bool(int)> key_pressed;

		void set_hovered_element(element* new_element);
		void set_focused_element(element* new_element);

		void on_element_delete(element& element);

	private:
		// used to check if set_focused_element was called from within itself
		bool in_set_focused_element = false;
		// in case of calling set_focused_element from itself, delay setting the new element
		// after the original set_focused_element's call events were sent
		element* next_focused_element = nullptr;
	};
}