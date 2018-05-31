#pragma once

#include "../input/input.h"
#include "../layout/layout.h"
#include "context.h"
#include <nanovg.h>


// return reference to the original type instead of the base class
template <typename T, typename Container, typename... Args>
T& emplace_back_derived(Container& container, Args& ... args)
{
	return static_cast<T&>(*container.emplace_back(std::make_unique<T>(args...)).get());
}


struct element : layout::element<element>, input::element
{
	inline static gui::context* context = nullptr;

	element* parent = nullptr;

	template<typename T, typename... Args>
	T& create_child(Args&... args) {
		auto& child = emplace_back_derived<T>(children, args...);
		child.parent = this;
		return child;
	}

	void draw() { draw(context->vg); }
	virtual void draw(NVGcontext* vg) {}

	void draw_recursive() { draw_recursive(context->vg); }
	void draw_recursive(NVGcontext* vg) {
		draw(vg);
		for (auto const& child : children)
			child->draw_recursive(vg);
	}
};

namespace gui::layout
{
	using box = ::layout::box<::element>;
}