#include "manager.h"

namespace input
{
	void manager::set_hovered_element(element* new_element)
	{
		if (new_element != hovered_element)
		{
			// hover end event
			if(hovered_element != nullptr)
				hover_end.send_focused_event(*hovered_element);

			hovered_element = new_element;

			// hover start event
			if (hovered_element != nullptr)
				hover_start.send_focused_event(*hovered_element, {});
		}
	}

	void manager::set_focused_element(element* new_element)
	{
		if(!in_set_focused_element) {
			in_set_focused_element = true;
			next_focused_element = nullptr;
			if (new_element != focused_element)
			{
				// focus end event
				if(focused_element != nullptr)
					focus_end.send_focused_event(*focused_element);

				// focus start event.
				// focus on the element that's subscribed to focus_start
				if (new_element != nullptr)
					focused_element = focus_start.send_focused_event(*new_element);
				else
					focused_element = nullptr;
			}

			in_set_focused_element = false;
			if(next_focused_element) {
				set_focused_element(next_focused_element);
			}
		}
		else {
			next_focused_element = new_element;
		}
	}

	void manager::on_element_delete(element& element)
	{
		if(event::callback_element == &element) {
			event::callback_element = nullptr; // mark as deleted
		}

		if (hovered_element == &element) {
			hovered_element = nullptr;
		}

		if (focused_element == &element) {
			focused_element = nullptr;
		}

		if (pressed_element == &element) {
			pressed_element = nullptr;
		}
	}
}