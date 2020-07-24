#pragma once

#include "element.h"

namespace layout
{
	// layout base class
	template <typename element_derived>
	struct base
	{
		virtual ~base() = default;

		virtual std::type_info const& type_info() const = 0;

		// owning element which uses this layout for its children
		element<element_derived>* parent = nullptr;

		virtual void fit() = 0;
		virtual void lay() = 0;
		virtual void perform() = 0;

		virtual void layout_complete()
		{
			for(auto& child : parent->children) {
				child->post_layout();
			}
		}
	};
}