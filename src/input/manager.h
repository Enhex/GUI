#pragma once

#include "../math/vector.h"
#include "element.h"
#include "events.h"

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

	public:
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


		void send_focused_event(element* element, size_t event_id, std::any&& args);

		void send_global_event(size_t event_id, std::any&& args);


		void set_hovered_element(element* new_element);

		decltype(focused_events) const& get_focused_events();
	};
}