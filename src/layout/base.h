#pragma once

#include "element.h"

namespace layout
{
	// layout base class
	template <typename derived_element>
	struct base
	{
		using element_t = element<derived_element>;

		virtual ~base() = default;

		virtual std::type_info const& type_info() const = 0;

		// owning element which uses this layout for its children
		element_t* parent = nullptr;

		virtual void fit() = 0;

		// allow expanding inside elements by default
		virtual void expand()
		{
			auto& parent = this->parent;

			std::vector<derived_element*> visible_children;
			for (auto& child : parent->children) {
				if(child->visible)
					visible_children.emplace_back(child.get());
			}

			for (auto& child : visible_children)
			{
				if (child->expand[0]) {
					child->size.a[0] = child->min_size.a[0];// reset expanders' size, so their expanded size won't be used.
					child->size.a[0] = std::max(child->size.a[0], parent->size.a[0]);
				}
				if (child->expand[1]) {
					child->size.a[1] = child->min_size.a[1];// reset expanders' size, so their expanded size won't be used.
					child->size.a[1] = std::max(child->size.a[1], parent->size.a[1]);
				}

				if (child->child_layout != nullptr)
					child->child_layout->expand();
			}
		}

		virtual void lay() = 0;
		virtual void perform() = 0;

		void layout_start_recurse(element_t& element)
		{
			if(!element.visible)
				return;

			element.pre_layout();
			for(auto& child : element.children) {
				layout_start_recurse(*child);
			}
		}
		virtual void layout_start()
		{
			layout_start_recurse(*parent);
		}

		void layout_complete_recurse(element_t& element)
		{
			if(!element.visible)
				return;

			element.post_layout();
			for(auto& child : element.children) {
				layout_complete_recurse(*child);
			}
		}
		virtual void layout_complete()
		{
			layout_complete_recurse(*parent);
		}
	};
}