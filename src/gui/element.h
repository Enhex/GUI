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
	inline static constexpr auto element_name{ "element" };
	virtual std::string get_element_name() { return element_name; }

	virtual std::type_info const& type_info() const { return typeid(element); }

	inline static gui::context* context = nullptr;

	element* parent = nullptr;

	std::string style = element_name;

	// get absolute position. regular `position` variable being relative to the parent
	vector2 get_position() {
		return parent != nullptr ? parent->get_position() + position : position;
	}

	bool is_inside(vector2 const& point) {
		rectangle absolute_rectangle{ get_position(), size };
		return absolute_rectangle.is_inside(point);
	}

	virtual void set_style(style::style_t const& style) {}

	inline void apply_style()
	{
		auto iter = context->style_manager.styles.find(style);
		if (iter != context->style_manager.styles.end()) {
			set_style(iter->second);
		}
	}

	template<typename T, typename... Args>
	T& create_child(Args&... args) {
		auto& child = emplace_back_derived<T>(children, args...);
		child.parent = this;
		return child;
	}

	void draw() { draw(context->vg); }
	virtual void draw(NVGcontext* vg) {}

	void draw_recursive() { draw_recursive(context->vg); }
	virtual void draw_recursive(NVGcontext* vg) {
		draw(vg);
		for (auto const& child : children)
			child->draw_recursive(vg);
	}
};

namespace gui::layout
{
	using box = ::layout::box<::element>;
}