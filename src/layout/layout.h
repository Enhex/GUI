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
		layout::base<derived>* layout = nullptr;
		std::array<bool, layout::orientation::count> expand{false};
	};


	// layout base class
	template <typename derived_element>
	struct base
	{
		derived_element* parent = nullptr;
	};

	template <typename derived_element>
	struct box : base<derived_element>
	{
		orientation orient = vertical;

		void layout()
		{
			//TODO can be pre-calcualted when adding/removing children
			auto free_space = parent->size.a[orient];
			size_t num_expanders = 0;

			for (auto const& child : parent->children)
			{
				// find free space for expansion
				if (child->expand[orient])
					++num_expanders;
				else
					free_space -= child->size.a[orient];
			}

			auto const expander_size = free_space / num_expanders;

			// size & position the children
			auto last_orient_position = parent->position.a[orient];
			for (auto& child : parent->children)
			{
				//TODO min size
				if (child->expand[horizontal])
					X(child->size) = orient == horizontal ? expander_size : X(parent->size);
				if (child->expand[vertical])
					Y(child->size) = orient == vertical ? expander_size : Y(parent->size);

				X(child->position) = orient == horizontal ? last_orient_position : X(parent->position);
				Y(child->position) = orient == vertical ? last_orient_position : Y(parent->position);

				last_orient_position += child->size.a[orient]; // advance to the current child's edge
			}
		}
	};
}