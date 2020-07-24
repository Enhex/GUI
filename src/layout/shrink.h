#pragma once

#include "base.h"

namespace layout
{
	// shrinks the parent to fit around its children, without positioning them.
	template <typename derived_element>
	struct shrink : base<derived_element>
	{
		std::type_info const& type_info() const override { return typeid(shrink); }

		// fit around children
		// size, bottom-up (need to know the size of the children to fit around them)
		void fit() override
		{
			auto& parent = *this->parent; // conformance: https://isocpp.org/wiki/faq/templates#nondependent-name-lookup-members
			auto& parent_rect = static_cast<rectangle&>(parent);

			// nothing to fit around
			if(parent.children.size() == 0)
				return;

			// initialize with the rectangle of the first element
			auto& first_child = parent.children.front();
			if (first_child->child_layout != nullptr)
				first_child->child_layout->perform();
			parent_rect = *first_child;
			
			
			for (auto& child : parent.children)
			{
				// update child layout
				if (child->child_layout != nullptr)
					child->child_layout->perform();

				// fit around the child
				parent_rect.merge(*child);
			}

			parent.min_size = parent.size;
		}

		// position, top-down (children need to know the parent's position to be relative to it)
		void lay() override
		{
			// do nothing
		}

		// perform layout
		void perform() override
		{
			// need to update size before updating position
			fit();
		}
	};
}