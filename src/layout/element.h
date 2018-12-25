#pragma once

#include "../math/math.h"
#include "orientation.h"
#include <array>
#include <memory>
#include <vector>

namespace layout
{
	template <typename derived_element>
	struct base;

	template <typename derived>
	struct element : rectangle
	{
		std::vector<std::unique_ptr<derived>> children;
		std::unique_ptr<layout::base<derived>> child_layout;
		std::array<bool, layout::orientation::count> expand{ false };

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
}