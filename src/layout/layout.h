#pragma once

#include <algorithm>
#include <array>
#include <boost/qvm/vec.hpp>
#include <boost/qvm/vec_access.hpp>
#include <boost/qvm/vec_operations.hpp>
#include <functional>
#include <memory>
#include <vector>

#include <nanovg.h>

namespace qvm = boost::qvm;

using vector2 = qvm::vec<float, 2>;


struct rectangle
{
	vector2 position{ 0,0 };
	vector2 size{ 0,0 };
};


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

		virtual void operator()() = 0;
	};

	template <typename derived_element>
	struct box : base<derived_element>
	{
		orientation orient = vertical;

		void operator()() override
		{
			//TODO can be pre-calcualted when adding/removing children
			float taken_space = 0;
			size_t num_expanders = 0;

			auto const non_orient = orient == horizontal ? vertical : horizontal;
			float non_orient_max_size = parent->min_size.a[non_orient];

			for (auto const& child : parent->children)
			{
				// find free space for expansion
				if (child->expand[orient])
					++num_expanders;
				else
					taken_space += child->min_size.a[orient];

				if(!child->expand[non_orient])
					non_orient_max_size = std::max(non_orient_max_size, child->min_size.a[non_orient]);
			}

			// fit around children
			if (!parent->expand[orient])
				parent->size.a[orient] = std::max(parent->min_size.a[orient], taken_space);
			if (!parent->expand[non_orient])
				parent->size.a[non_orient] = non_orient_max_size;

			auto const free_space = parent->size.a[orient] - taken_space;
			auto const expander_size = free_space / num_expanders;

			// size & position the children
			auto last_orient_position = parent->position.a[orient];
			for (auto& child : parent->children)
			{
				if (child->expand[orient])
					child->size.a[orient] = expander_size;
				if (child->expand[non_orient])
					child->size.a[non_orient] = parent->size.a[non_orient];
				
				// min size
				X(child->size) = std::max(X(child->size), X(child->min_size));
				Y(child->size) = std::max(Y(child->size), Y(child->min_size));

				// position relative to parent/previous child
				X(child->position) = orient == horizontal ? last_orient_position : X(parent->position);
				Y(child->position) = orient == vertical ? last_orient_position : Y(parent->position);

				last_orient_position += child->size.a[orient]; // advance to the current child's edge

				if (child->child_layout != nullptr)
					(*child->child_layout)();
			}
		}
	};
}