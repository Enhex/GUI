#pragma once

#include "../math/math.h"
#include <any>
#include <array>
#include <functional>
#include <limits>
#include <unordered_map>
#include <unordered_set>

namespace input
{
	namespace event
	{
		auto static constexpr invalid_id = std::numeric_limits<size_t>::max();

		struct base
		{
			virtual size_t get_id() = 0;
			//TODO name could be useful, especially for interpreting events from data files
		};

		struct key_release : base
		{
			static constexpr size_t id = 0;
			size_t get_id() const { return id; }

			// key, mods
			using params = std::tuple<int, int>;
		};

		struct key_press : base
		{
			static constexpr size_t id = 1;
			size_t get_id() const { return id; }

			// key, mods
			using params = std::tuple<int, int>;
		};

		struct hover_start : base
		{
			static constexpr size_t id = 2;
			size_t get_id() const { return id; }

			// mouse_position
			using params = std::tuple<vector2 const&>;
		};

		struct hover_end : base
		{
			static constexpr size_t id = 3;
			size_t get_id() const { return id; }

			// mouse_position
			//using params = std::tuple<vector2 const&>;
		};
	}

	struct element
	{
		/*TODO
		- focus
		- event handling
		- doesn't have hardcoded shape. derived classes need to handle it (click detection and such)
		*/
		//element* parent = nullptr;
	};

	using any_callback = std::function<void(std::any&&)>;

	/*TODO
	- global events context
	- unsub from events
	*/
	struct manager
	{
		element* focused_element = nullptr; // keyboard focus
		element* hovered_element = nullptr; // mouse focus

		// map from element to subscribed events
		std::unordered_map<element*, std::unordered_map<size_t, any_callback>> focused_events;

		// map from event to callbacks
		std::unordered_map<size_t, std::vector<any_callback>> global_events;
		//TODO keep track of which callback belongs to which element, in a separate container
		//global_subscribers;


		// focused
		template<typename Event>
		void subscribe(element* subscriber, std::function<void(std::any&&)>&& callback)
		{
			//TODO check if element is already subscribed?
			focused_events[subscriber][Event::id] = std::move(callback);
		}

		// global
		template<typename Event>
		void subscribe_global(element* subscriber, std::function<void(std::any&&)>&& callback)
		{
			//TODO check if element is already subscribed?
			global_events[Event::id].emplace_back(std::move(callback));
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
				}
			}
		}

		void send_global_event(size_t event_id, std::any&& args)
		{
			for (auto const& callback : global_events[event_id]) {
				if (callback) {
					// call callback
					callback(std::move(args));
				}
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
	};
}