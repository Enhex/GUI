#pragma once

#include "base.h"

namespace layout
{
	// continue recursing to chilren's layouts without doing anything
	template <typename derived_element>
	struct forward : base<derived_element>
	{
		std::type_info const& type_info() const override { return typeid(forward); }

		void fit() override
		{
			auto& parent = *this->parent; // conformance: https://isocpp.org/wiki/faq/templates#nondependent-name-lookup-members

			for (auto& child : parent.children)
			{
				if (child->child_layout != nullptr)
					child->child_layout->fit();
			}
		}

		void lay() override
		{
			auto& parent = *this->parent;

			for (auto& child : parent.children)
			{
				if (child->child_layout != nullptr)
					child->child_layout->lay();
			}
		}

		// perform layout
		void perform() override
		{
			this->layout_start();

			fit();
			lay();

			this->layout_complete();
		}
	};
}