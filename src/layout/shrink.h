#pragma once

#include "base.h"

namespace layout
{
	// shrinks the parent to fit around its children, without positioning them.
	template <typename derived_element>
	struct shrink : base<derived_element>
	{
		std::type_info const& type_info() const override { return typeid(shrink); }

		// should the parent change its position too, or only its size?
		bool shrink_position = true;

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
			if(shrink_position)
				parent_rect = *first_child;
			else
				parent_rect.size = first_child->size;

			for (auto& child : parent.children)
			{
				// update child layout
				if (child->child_layout != nullptr)
					child->child_layout->perform();

				// prevent expanding children from blowing up the size
				auto non_expand_size = child->min_size;
				if(child->expand[layout::horizontal] == false)
					X(non_expand_size) = X(child->size);
				if(child->expand[layout::vertical] == false)
					Y(non_expand_size) = Y(child->size);

				// fit around the child
				if(shrink_position) {
					parent_rect.merge(child->position);
					parent_rect.merge(non_expand_size);
				}
				else {
					X(parent_rect.size) = std::max(X(parent_rect.size), X(non_expand_size) + X(child->position));
					Y(parent_rect.size) = std::max(Y(parent_rect.size), Y(non_expand_size) + Y(child->position));
				}
			}

			parent.min_size = parent.size;
		}

		void lay() override
		{
			// do nothing
		}

		// perform layout
		void perform() override
		{
			this->layout_start();

			fit();

			this->layout_complete();
		}
	};
}