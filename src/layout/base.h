#pragma once

#include "element.h"

namespace layout
{
	// layout base class
	template <typename element_derived>
	struct base
	{
		using element_t = element<element_derived>;

		virtual ~base() = default;

		virtual std::type_info const& type_info() const = 0;

		// owning element which uses this layout for its children
		element_t* parent = nullptr;

		virtual void fit() = 0;
		virtual void lay() = 0;
		virtual void perform() = 0;

		void layout_start_recurse(element_t& element)
		{
			if(!element.visible)
				return;

			element.pre_layout();
			for(auto& child : element.children) {
				layout_start_recurse(*child);
			}
		}
		virtual void layout_start()
		{
			layout_start_recurse(*parent);
		}

		void layout_complete_recurse(element_t& element)
		{
			if(!element.visible)
				return;

			element.post_layout();
			for(auto& child : element.children) {
				layout_complete_recurse(*child);
			}
		}
		virtual void layout_complete()
		{
			layout_complete_recurse(*parent);
		}
	};
}