#pragma once

#include "element.h"

namespace layout
{
	// layout base class
	template <typename element_derived>
	struct base
	{
		// owning element which uses this layout for its children
		element<element_derived>* parent = nullptr;

		virtual void fit() = 0;
		virtual void operator()() = 0;
	};
}