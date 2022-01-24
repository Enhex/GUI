#pragma once

#include "../math/vector.h"
#include "element.h"
#include "events.h"

namespace input
{
	struct element;

	/*TODO
	- global events context that the user can switch between
	*/
	struct manager
	{
		vector2 mouse_pos;

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
		event::scroll scroll;
		event::character character;
		event::frame_start frame_start;
		event::focus_start focus_start;
		event::focus_end focus_end;
		event::visible_start visible_start;
		event::visible_end visible_end;

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