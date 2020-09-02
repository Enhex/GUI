#pragma once

#include "../math/math.h"
#include "element.h"
#include <any>
#include <functional>
#include <unordered_map>

namespace input
{
	struct element;

	using any_callback = std::function<void(std::any&&)>;

	/*TODO
	- global events context that the user can switch between
	*/
	struct manager
	{
		using event_ID = size_t;

		vector2 mouse_pos;

		element* focused_element = nullptr; // keyboard focus
		element* hovered_element = nullptr; // mouse focus
		element* pressed_element = nullptr; // element which was hovered when mouse press happened

		// focused
		template<typename Event>
		void subscribe(element* subscriber, std::function<void(std::any&&)>&& callback)
		{
			// add removal callback
			if (focused_events.find(subscriber) != focused_events.end())
			{
				subscriber->destructor_callbacks.emplace_back([this, subscriber]() {
					focused_events.erase(subscriber);
				});
			}

			// subscribe
			focused_events[subscriber][Event::id] = std::move(callback);
		}

		// global
		template<typename Event>
		void subscribe_global(element* subscriber, std::function<void(std::any&&)>&& callback)
		{
			auto& event = global_events[Event::id];

			// add removal callback
			if (event.find(subscriber) != event.end())
			{
				subscriber->destructor_callbacks.emplace_back([this, subscriber]() {
					global_events[Event::id].erase(subscriber);
				});
			}

			// subscribe
			event[subscriber] = std::move(callback);
		}

		// focused
		template<typename Event>
		void unsubscribe(element* subscriber)
		{
			auto iter = focused_events.find(subscriber);
			if(iter != focused_events.end())
				iter->second.erase(Event::id);
		}

		// global
		template<typename Event>
		void unsubscribe_global(element* subscriber)
		{
			auto& event = global_events[Event::id];

			if (active_callback_element == subscriber &&
				active_callback_event_id == Event::id)
			{
				// if self-unsubscribing, update iterator
				active_callback_global_iter = event.erase(event.find(subscriber));
			}
			else {
				event.erase(subscriber);
			}
		}


		void send_focused_event(element* element, size_t event_id, std::any&& args)
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

		void send_global_event(size_t event_id, std::any&& args)
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


		void set_hovered_element(element* new_element)
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

		auto const& get_focused_events()
		{
			return focused_events;
		}

	protected:
		// map from element to subscribed events
		std::unordered_map<element*, std::unordered_map<event_ID, any_callback>> focused_events;
		//NOTE: while technically it isn't a requirement for a subscriber to be an element (unlike focused event which listens to the subscribing element's events), it's
		// still useful for automatic unsubscription on destruction and unified API.
		std::unordered_map<event_ID, std::unordered_map<element*, any_callback>> global_events;

		// used to handle deleting subsription while iterating
		element* active_callback_element = nullptr;
		event_ID active_callback_event_id = -1;
		std::unordered_map<element*, any_callback>::iterator active_callback_global_iter;
	};
}