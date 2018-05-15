#pragma once

#include <algorithm>
#include <array>
#include <boost/qvm/vec.hpp>
#include <boost/qvm/vec_access.hpp>
#include <boost/qvm/vec_operations.hpp>
#include <vector>
#include <functional>

namespace qvm = boost::qvm;

using vector2 = qvm::vec<float, 2>;


struct rectangle
{
	vector2 position{ 0,0 };
	vector2 size{ 0,0 };
};

// node in the layout tree
struct node
{
};

namespace layout
{
	enum orientation {
		horizontal,
		vertical,
		count
	};

	struct base;
}

struct element : node, rectangle
{
	std::vector<element> children; //TODO should be layout base pointer?
	layout::base* layout = nullptr;
	std::array<bool, layout::orientation::count> expand{false};
};

namespace layout
{
	struct base : node
	{
		element* parent = nullptr;
	};

	struct box : base
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
				if (child.expand[orient])
					++num_expanders;
				else
					free_space -= child.size.a[orient];
			}

			auto const expander_size = free_space / num_expanders;

			// size & position the children
			auto last_orient_position = parent->position.a[orient];
			for (auto& child : parent->children)
			{
				//TODO min size
				if (child.expand[horizontal])
					X(child.size) = orient == horizontal ? expander_size : X(parent->size);
				if (child.expand[vertical])
					Y(child.size) = orient == vertical ? expander_size : Y(parent->size);

				X(child.position) = orient == horizontal ? last_orient_position : X(parent->position);
				Y(child.position) = orient == vertical ? last_orient_position : Y(parent->position);

				last_orient_position += child.size.a[orient]; // advance to the current child's edge
			}
		}
	};
}