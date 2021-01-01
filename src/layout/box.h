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
			auto& parent = this->parent; // conformance: https://isocpp.org/wiki/faq/templates#nondependent-name-lookup-members
			auto const non_orient = orient == horizontal ? vertical : horizontal;

			// use the maximum child size on the non-orient axis
			float non_orient_max_size = parent->min_size.a[non_orient];
			// total children size on the orient axis
			float children_size = 0;

			std::vector<derived_element*> visible_children;
			for (auto& child : parent->children) {
				if(child->visible)
					visible_children.emplace_back(child.get());
			}

			// apply child layout fit
			for (auto& child : visible_children)
			{
				// reset expanders' size, so their expanded size won't be used.
				if (child->expand[orient])
					child->size.a[orient] = child->min_size.a[orient];
				if (child->expand[non_orient])
					child->size.a[non_orient] = child->min_size.a[non_orient];

				if (child->child_layout != nullptr)
					child->child_layout->fit();

				children_size += child->size.a[orient];
				non_orient_max_size = std::max(non_orient_max_size, child->size.a[non_orient]);
			}

			// set parent size
			parent->size.a[orient] = std::max(parent->min_size.a[orient], children_size);
			parent->size.a[non_orient] = non_orient_max_size; // already initialized with parent min_size
		}

		// after parent's size is known, children can expand inside it.
		void expand() override
		{
			auto& parent = this->parent;
			auto const non_orient = orient == horizontal ? vertical : horizontal;

			// use the space taken by non-expanding children for the orient axis //TODO testing another option
			float taken_space = 0;
			// number of children that expand in the orient direction
			size_t num_expanders = 0;

			std::vector<derived_element*> visible_children;
			for (auto& child : parent->children) {
				if(child->visible)
					visible_children.emplace_back(child.get());
			}

			for (auto& child : visible_children)
			{
				if (!child->expand[orient]) {
					taken_space += child->size.a[orient];
				}
				else {
					++num_expanders;
				}
			}

			// expand children after the parent's size is known
			auto const free_space = std::max(0.f, parent->size.a[orient] - taken_space);
			auto const expander_size = num_expanders == 0 ? 0 : free_space / num_expanders;

			for (auto& child : visible_children)
			{
				if (child->expand[orient])
					child->size.a[orient] = std::max(child->size.a[orient], expander_size);
				if (child->expand[non_orient])
					child->size.a[non_orient] = std::max(child->size.a[non_orient], parent->size.a[non_orient]);

				if (child->child_layout != nullptr)
					child->child_layout->expand();
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
				if(!child->visible)
					continue;

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
			expand();
			lay();

			this->layout_complete();
		}
	};
}