#pragma once

#include "../math/math.h"
#include <algorithm>
#include <array>
#include <functional>
#include <memory>
#include <nanovg.h>
#include <vector>


namespace layout
{
	enum orientation {
		horizontal,
		vertical,
		count
	};


	template <typename derived_element>
	struct base;

	template <typename derived>
	struct element : rectangle
	{
		std::vector<std::unique_ptr<derived>> children; //TODO should be layout base pointer?
		std::unique_ptr<layout::base<derived>> child_layout;
		std::array<bool, layout::orientation::count> expand{false};

		vector2 min_size{ 0,0 };

		void apply_min_size()
		{
			X(size) = std::max(X(size), X(min_size));
			Y(size) = std::max(Y(size), Y(min_size));
		}

		template <typename T>
		T& create_layout()
		{
			child_layout = std::make_unique<T>();
			child_layout->parent = this;
			return static_cast<T&>(*child_layout.get());
		}

		void set_layout(layout::base<derived>* new_layout)
		{
			child_layout.reset(new_layout);
			child_layout->parent = this;
		}
	};


	// layout base class
	template <typename element_derived>
	struct base
	{
		// owning element which uses this layout for its children
		element<element_derived>* parent = nullptr;

		virtual void fit() = 0;
		virtual void operator()() = 0;
	};

	template <typename derived_element>
	struct box : base<derived_element>
	{
		orientation orient = vertical;

		float taken_space = 0;

		// fit around children
		void fit() override
		{
			auto& parent = this->parent; // conformance: https://isocpp.org/wiki/faq/templates#nondependent-name-lookup-members

			taken_space = 0;
			
			auto const non_orient = orient == horizontal ? vertical : horizontal;
			float non_orient_max_size = parent->min_size.a[non_orient];

			for (auto& child : parent->children)
			{
				if (child->child_layout != nullptr)
					child->child_layout->fit();
				else
					child->apply_min_size(); // make sure the size is updated

				if (!child->expand[orient])
					taken_space += std::max(child->size.a[orient], child->size.a[orient]);
				if (!child->expand[non_orient])
					non_orient_max_size = std::max(non_orient_max_size, child->size.a[non_orient]);
			}

			if (!parent->expand[orient])
				parent->size.a[orient] = std::max(parent->min_size.a[orient], taken_space);
			if (!parent->expand[non_orient])
				parent->size.a[non_orient] = non_orient_max_size; // already started from min_size
		}

		// perform layout
		void operator()() override
		{
			auto& parent = this->parent;

			//TODO can be pre-calcualted when adding/removing children
			size_t num_expanders = 0;

			auto const non_orient = orient == horizontal ? vertical : horizontal;

			for (auto const& child : parent->children)
			{
				// find free space for expansion
				if (child->expand[orient])
					++num_expanders;
			}

			auto const free_space{ parent->size.a[orient] - taken_space };
			auto const expander_size = free_space / num_expanders;

			// size & position the children
			auto last_orient_position = parent->position.a[orient];
			for (auto& child : parent->children)
			{
				if (child->expand[orient])
					child->size.a[orient] = expander_size;
				if (child->expand[non_orient])
					child->size.a[non_orient] = parent->size.a[non_orient];

				// position relative to parent/previous child
				X(child->position) = orient == horizontal ? last_orient_position : X(parent->position);
				Y(child->position) = orient == vertical ? last_orient_position : Y(parent->position);

				last_orient_position += child->size.a[orient]; // advance to the current child's end position

				if (child->child_layout != nullptr)
					(*child->child_layout)();
			}
		}
	};
}