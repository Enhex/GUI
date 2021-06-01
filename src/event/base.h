#pragma once

#include "../input/element.h"

namespace input
{
	namespace event
	{
		extern element* callback_element;
		extern element* active_callback_element;

		template<typename... Args>
		struct base
		{
			using callback_t = std::function<void(Args...)>;
			using callbacks_t = std::list<callback_t>;

			typename std::unordered_map<element*, callback_t>::iterator active_callback_global_iter;

			// focused
			typename callbacks_t::iterator subscribe(element* subscriber, callback_t&& callback)
			{
				// subscribe
				auto& callbacks = focused_events[subscriber];
				callbacks.emplace_back(std::move(callback));

				// add removal callback
				subscriber->destructor_callbacks.emplace_back([this, subscriber]() {
					focused_events.erase(subscriber);
				});

				return --callbacks.end();
			}

			void subscribe_global(element* subscriber, callback_t&& callback)
			{
				// subscribe
				global_event[subscriber] = std::move(callback);

				// add removal callback
				subscriber->destructor_callbacks.emplace_back([this, subscriber]() {
					global_event.erase(subscriber);
				});
			}

			// return false if the event already has exclusive subscription
			bool subscribe_exclusive(callback_t&& callback)
			{
				if(exclusive_event.has_value())
					return false;

				exclusive_event = callback;
				return true;
			}


			// focused
			void unsubscribe(element* subscriber)
			{
				focused_events.erase(subscriber);
			}

			void unsubscribe_global(element* subscriber)
			{
				if (active_callback_element == subscriber) {
					// if self-unsubscribing, update iterator
					auto sub_iter = global_event.find(subscriber);
					if(sub_iter != global_event.end())
						active_callback_global_iter = global_event.erase(sub_iter);
				}
				else {
					global_event.erase(subscriber);
				}
			}

			void unsubscribe_exclusive()
			{
				exclusive_event.reset();
			}


			void send_event(element* element, Args... args)
			{
				// try exclusive first
				if(exclusive_event.has_value()) {
					// call callback
					exclusive_event.value()(args...);
					return;
				}

				// try focused
				if(element)
					send_focused_event(*element, args...);

				// no focused callback was found, propagate into a global event
				send_global_event(args...);
			}

			element* send_focused_event(element& element, Args... args)
			{
				// find element
				auto event_iter = focused_events.find(&element);
				if (event_iter != focused_events.end()) {
					// call the callbacks
					callback_element = &element;
					for(auto& callback : event_iter->second) {
						callback(args...);
						// stop calling callbacks if the element was deleted
						if(callback_element == nullptr)
							return nullptr;
					}
					return &element;
				}

				if(element.get_parent() != nullptr)
					return send_focused_event(*element.get_parent(), args...);

				return nullptr;
			}

			void send_global_event(Args... args)
			{
				// call the callback of all of the global event's subscribers
				for (active_callback_global_iter = global_event.begin(); active_callback_global_iter != global_event.end(); ++active_callback_global_iter) {
					auto& [element, callback] = *active_callback_global_iter;

					// set active callback
					active_callback_element = element;

					// call callback
					callback(args...);

					// unset active callback
					active_callback_element = nullptr;

					// in case of self-unsubscribe of the last iterator, to avoid trying to increment invalid iterator
					if (active_callback_global_iter == global_event.end())
						break;
				}
			}

		protected:
			// map from element to subscribed events
			//NOTE: using list for stable iterators
			std::unordered_map<element*, callbacks_t> focused_events;
			//NOTE: while technically it isn't a requirement for a subscriber to be an element (unlike focused event which listens to the subscribing element's events), it's
			// still useful for automatic unsubscription on destruction and unified API.
			std::unordered_map<element*, callback_t> global_event;
			// only one callback per event
			std::optional<callback_t> exclusive_event;
		};
	}
}