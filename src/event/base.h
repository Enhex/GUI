#pragma once

#include "../input/element.h"

#include <optional>

namespace input
{
	namespace event
	{
		extern element* callback_element;
		extern element* active_callback_element;
		// used to stop propagating to the rest of the callbacks
		extern bool stop_callbacks;

		template<typename... Args>
		struct base
		{
			using callback_t = std::function<void(Args...)>;
			using callbacks_t = std::list<callback_t>;

			// used for self removal
			typename std::unordered_map<element*, callback_t>::iterator active_callback_global_iter;
			typename std::unordered_map<element*, callback_t>::iterator active_callback_global_unfocused_iter;

			// focused
			template<bool first=false>
			typename callbacks_t::iterator subscribe(element* subscriber, callback_t&& callback)
			{
				// subscribe
				auto& callbacks = focused_events[subscriber];
				if constexpr(first){
					callbacks.emplace_front(std::move(callback));
				}
				else{
					callbacks.emplace_back(std::move(callback));
				}

				// add removal callback
				subscriber->destructor_callbacks.emplace_back([this, subscriber]() {
					focused_events.erase(subscriber);
				});
				auto dtor_callback_iter = --subscriber->destructor_callbacks.end();
				focused_events_dtor_callbacks[subscriber] = dtor_callback_iter;

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
				auto dtor_callback_iter = --subscriber->destructor_callbacks.end();
				global_events_dtor_callbacks[subscriber] = dtor_callback_iter;
			}

			void subscribe_global_unfocused(element* subscriber, callback_t&& callback)
			{
				// subscribe
				global_unfocused_event[subscriber] = std::move(callback);

				// add removal callback
				subscriber->destructor_callbacks.emplace_back([this, subscriber]() {
					global_unfocused_event.erase(subscriber);
				});
				auto dtor_callback_iter = --subscriber->destructor_callbacks.end();
				global_unfocused_events_dtor_callbacks[subscriber] = dtor_callback_iter;
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
				// remove dtor callback
				auto dtor_callback_iter = focused_events_dtor_callbacks.find(subscriber);
				if(dtor_callback_iter != focused_events_dtor_callbacks.end()){
					subscriber->destructor_callbacks.erase(dtor_callback_iter->second);
					focused_events_dtor_callbacks.erase(subscriber);
				}
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

				// remove dtor callback
				auto dtor_callback_iter = global_events_dtor_callbacks.find(subscriber);
				if(dtor_callback_iter != global_events_dtor_callbacks.end()){
					subscriber->destructor_callbacks.erase(dtor_callback_iter->second);
					global_events_dtor_callbacks.erase(subscriber);
				}
			}

			void unsubscribe_global_unfocused(element* subscriber)
			{
				if (active_callback_element == subscriber) {
					// if self-unsubscribing, update iterator
					auto sub_iter = global_unfocused_event.find(subscriber);
					if(sub_iter != global_unfocused_event.end())
						active_callback_global_unfocused_iter = global_unfocused_event.erase(sub_iter);
				}
				else {
					global_unfocused_event.erase(subscriber);
				}

				// remove dtor callback
				auto dtor_callback_iter = global_unfocused_events_dtor_callbacks.find(subscriber);
				if(dtor_callback_iter != global_unfocused_events_dtor_callbacks.end()){
					subscriber->destructor_callbacks.erase(dtor_callback_iter->second);
					global_unfocused_events_dtor_callbacks.erase(subscriber);
				}
			}

			void unsubscribe_exclusive()
			{
				exclusive_event.reset();
			}

			void set_continue_propagating(element* subscriber, bool const propagate)
			{
				if(propagate){
					continue_propagating.insert(subscriber);

					// add removal callback
					subscriber->destructor_callbacks.emplace_back([this, subscriber]() {
						continue_propagating.erase(subscriber);
					});
					auto dtor_callback_iter = --subscriber->destructor_callbacks.end();
					propagate_dtor_callbacks[subscriber] = dtor_callback_iter;
				}
				else{
					continue_propagating.erase(subscriber);
					// remove dtor callback
					auto dtor_callback_iter = propagate_dtor_callbacks.find(subscriber);
					if(dtor_callback_iter != propagate_dtor_callbacks.end()){
						subscriber->destructor_callbacks.erase(dtor_callback_iter->second);
						propagate_dtor_callbacks.erase(subscriber);
					}
				}
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
				if(element){
					auto* recieve_element = send_focused_event(*element, args...);
					send_global_unfocused_event(args...);
					if(recieve_element != nullptr &&
					   continue_propagating.count(recieve_element) == 0)
					{
						return;
					}
				}

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
						stop_callbacks = false;
						callback(args...);
						// stop calling callbacks if the element was deleted
						if(callback_element == nullptr)
							return nullptr;
						// stop callbacks if the called callback requested it
						if(stop_callbacks){
							break;
						}
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

			void send_global_unfocused_event(Args... args)
			{
				// call the callback of all of the global event's subscribers
				for (active_callback_global_unfocused_iter = global_unfocused_event.begin();
				     active_callback_global_unfocused_iter != global_unfocused_event.end(); ++active_callback_global_unfocused_iter){
					auto& [element, callback] = *active_callback_global_unfocused_iter;

					// set active callback
					active_callback_element = element;

					// call callback
					callback(args...);

					// unset active callback
					active_callback_element = nullptr;

					// in case of self-unsubscribe of the last iterator, to avoid trying to increment invalid iterator
					if (active_callback_global_unfocused_iter == global_event.end())
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
			// executes even if an element already captured the event (non-exclusive)
			std::unordered_map<element*, callback_t> global_unfocused_event;
			// only one callback per event
			std::optional<callback_t> exclusive_event;

			// elements that let the event continue propagating
			std::unordered_set<element*> continue_propagating;

			// store destructor callback iterators to remove them when unsubscribing
			std::unordered_map<element*, decltype(element::destructor_callbacks)::iterator> focused_events_dtor_callbacks;
			std::unordered_map<element*, decltype(element::destructor_callbacks)::iterator> global_events_dtor_callbacks;
			std::unordered_map<element*, decltype(element::destructor_callbacks)::iterator> global_unfocused_events_dtor_callbacks;
			std::unordered_map<element*, decltype(element::destructor_callbacks)::iterator> propagate_dtor_callbacks;
		};
	}
}