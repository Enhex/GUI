#pragma once

#include "base.h"
#include "orientation.h"

namespace layout
{
	// automatically lays out children vertically/horizontally, with an option for them to expand.
	// expansion space is divides equally between the expanders.
	template <typename derived_element>
	struct box : base<derived_element>
	{
		std::type_info const& type_info() const override { return typeid(box); }

		orientation orient = vertical;

		// fit around children
		// size, bottom-up (need to know the size of the children to fit around them)
		void fit() override
		{
			/*			
			- find expander size
			- set expanders' size
			- fit children (expanders don't shrink) 
			- if element isn't an expander, set size to children's size (shrink). 
			in fact if there's a single child expander, the element won't shrink.
			*/

			//TODO things can be pre-calcualted when adding/removing children

			auto& parent = this->parent; // conformance: https://isocpp.org/wiki/faq/templates#nondependent-name-lookup-members
			auto const non_orient = orient == horizontal ? vertical : horizontal;

			// use the maximum child size on the non-orient axis
			float non_orient_max_size = std::max(parent->size.a[non_orient], parent->min_size.a[non_orient]);
			// total children size on the orient axis
			float children_size = 0;
			// use the space taken by non-expanding children for the orient axis //TODO testing another option
			float taken_space = 0;
			// number of children that expand in the orient direction
			size_t num_expanders = 0;

			for (auto& child : parent->children)
			{
				if (!child->expand[orient]) {
					child->apply_min_size();
					taken_space += child->size.a[orient];
				}
				else {
					++num_expanders;
				}

				// sample child's size
				non_orient_max_size = std::max(non_orient_max_size, child->size.a[non_orient]);
			}


			// expand children after the parent's size is known
			auto const free_space{ parent->size.a[orient] - taken_space };
			auto const expander_size = num_expanders == 0 ? 0 : free_space / num_expanders;

			for (auto& child : parent->children)
			{
				if (child->expand[orient])
					child->size.a[orient] = std::max(child->size.a[orient], expander_size);
				if (child->expand[non_orient])
					child->size.a[non_orient] = std::max(child->size.a[non_orient], parent->size.a[non_orient]);
			}


			// shrink non-expanding children
			for (auto& child : parent->children)
			{
				if (child->child_layout != nullptr)
					child->child_layout->fit();
				
				children_size += child->size.a[orient];
			}


			// set parent size
			if (num_expanders < 1) // only shrink if there are no expander children
			{
				if(!parent->expand[orient])
					parent->size.a[orient] = std::max(parent->min_size.a[orient], children_size);
				if(!parent->expand[non_orient])
					parent->size.a[non_orient] = non_orient_max_size; // already started from min_size
			}
		}

		// position, top-down (children need to know the parent's position to be relative to it)
		void lay() override
		{
			auto& parent = this->parent;

			// position the children
			auto last_orient_position = 0.f;
			for (auto& child : parent->children)
			{
				// position relative to parent/previous child
				X(child->position) = orient == horizontal ? last_orient_position : 0.f;
				Y(child->position) = orient == vertical ? last_orient_position : 0.f;

				last_orient_position += child->size.a[orient]; // advance to the current child's end position

				if (child->child_layout != nullptr)
					child->child_layout->lay();
			}
		}

		// perform layout
		void perform() override
		{
			this->layout_start();

			// need to update size before updating position
			fit();
			lay();

			this->layout_complete();
		}
	};
}