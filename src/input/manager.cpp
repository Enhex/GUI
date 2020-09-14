#include "manager.h"

namespace input
{
	void manager::send_event(element* element, size_t event_id, std::any&& args)
	{
		// try exclusive first
		auto iter = exclusive_events.find(event_id);
		if(iter != exclusive_events.end()) {
				// call callback
				iter->second(std::move(args));
				return;
		}

		// try focused
		send_focused_event(element, event_id, std::move(args));
	}

	void manager::send_focused_event(element* element, size_t event_id, std::any&& args)
	{
		// find element
		auto iter = focused_events.find(element);
		if (iter != focused_events.end()) {
			auto const& subscribed_events = iter->second;
			// find if element subscribed to the event
			auto event_iter = subscribed_events.find(event_id);
			if (event_iter != subscribed_events.end()) {
				// call callback
				event_iter->second(std::move(args));
				return;
			}
		}

		// no focused callback was found, propagate into a global event
		send_global_event(event_id, std::move(args));
	}

	void manager::send_global_event(size_t event_id, std::any&& args)
	{
		auto& event = global_events[event_id];
		for (active_callback_global_iter = event.begin(); active_callback_global_iter != event.end(); ++active_callback_global_iter) {
			auto& [element, callback] = *active_callback_global_iter;

			// set active callback
			active_callback_element = element;
			active_callback_event_id = event_id;

			// call callback
			callback(std::move(args));

			// unset active callback
			active_callback_element = nullptr;
			active_callback_event_id = -1;

			// in case of self-unsubscribe of the last iterator, to avoid trying to increment invalid iterator
			if (active_callback_global_iter == event.end())
				break;
		}
	}

	void manager::set_hovered_element(element* new_element)
	{
		if (new_element != hovered_element)
		{
			// hover end event
			if(hovered_element != nullptr)
				send_focused_event(hovered_element, event::hover_end::id, {});

			hovered_element = new_element;

			// hover start event
			if (hovered_element != nullptr)
				send_focused_event(hovered_element, event::hover_start::id, {});
		}
	}

	decltype(manager::focused_events) const& manager::get_focused_events()
	{
		return focused_events;
	}
}