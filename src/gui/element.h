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

	std::string style = element_name;

protected:
	std::string id;
	static std::unordered_map<std::string, element&> id_to_element;
public:

	element();
	virtual ~element();

	// self remove
	void remove();

	static element* get_by_id(std::string const& id);
	void set_id(std::string_view const new_id);
	std::string const& get_id();

	element* get_parent() override;

	void set_visible(bool new_visible) override;

	// get absolute position. regular `position` variable being relative to the parent
	vector2 get_position();

	bool is_inside(vector2 const& point);

	virtual void set_style(style::style_t const& style) {}

	void apply_style();

	template<typename T, typename... Args>
	T& create_child(Args&&... args) {
		auto& child = emplace_back_derived<T>(children, args...);
		child.parent = this;
		return child;
	}

	//NOTE: takes ownership of the child element
	void add_child(std::unique_ptr<element> child);
	// add child before position
	void add_child(std::unique_ptr<element> child, size_t position);

	void draw();
	virtual void draw(NVGcontext* vg) {}

	void draw_recursive();
	virtual void draw_recursive(NVGcontext* vg);

	bool is_focused() const;
	bool is_hovered() const;

	void focus();
};

namespace gui::layout
{
	using box = ::layout::box<::element>;
	using shrink = ::layout::shrink<::element>;
	using forward = ::layout::forward<::element>;
}