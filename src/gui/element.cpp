#include "element.h"


std::unordered_map<std::string, element&> element::id_to_element;

element::element()
{
	// default to forwarding
	create_layout<gui::layout::forward>();
}

element::~element()
{
	for(auto& child : children) {
		child->parent = nullptr;
	}

	context->input_manager.on_element_delete(*this);

	id_to_element.erase(id);
}

void element::remove()
{
	// keep a copy of the parent pointer because it needs to be set to null before deleting
	auto temp_parent = get_parent();
	parent = nullptr;

	for(auto iter = temp_parent->children.begin(); iter != temp_parent->children.end(); ++iter)
	{
		if(iter->get() == this) {
			temp_parent->children.erase(iter);
			return;
		}
	}
}
element* element::get_by_id(std::string const& id)
{
	auto iter = id_to_element.find(id);
	if(iter == id_to_element.end())
		return nullptr;

	return &iter->second;
}

void element::set_id(std::string_view const new_id)
{
	//if(id_to_element.find(new_id) != id_to_element.end())
	//	std::cerr << "warning: element ID already exists.";
	id = new_id;
	id_to_element.emplace(id, *this);
}

std::string const& element::get_id()
{
	return id;
}

element* element::get_parent()
{
	return static_cast<element*>(parent);
}

// get absolute position. regular `position` variable being relative to the parent
vector2 element::get_position() {
	return parent != nullptr ? get_parent()->get_position() + position : position;
}

bool element::is_inside(vector2 const& point) {
	rectangle absolute_rectangle{ get_position(), size };
	return absolute_rectangle.is_inside(point);
}

void element::apply_style()
{
	{
		auto iter = context->style_manager.styles.find(style);
		if (iter != context->style_manager.styles.end()) {
			set_style(iter->second);
		}
	}
	{
		auto iter = context->style_manager.styles.find(id);
		if (iter != context->style_manager.styles.end()) {
			set_style(iter->second);
		}
	}
}

void element::add_child(std::unique_ptr<element> child)
{
	child->parent = this;
	children.emplace_back(std::move(child));
}

void element::add_child(std::unique_ptr<element> child, size_t position)
{
	child->parent = this;
	auto iter = children.begin();
	if(position == children.size()) {
		children.emplace_back(std::move(child));
	}
	else {
		std::advance(iter, position);
		children.emplace(iter, std::move(child));
	}
}

void element::draw()
{
	draw(context->vg);
}

void element::draw_recursive()
{
	draw_recursive(context->vg);
}

void element::draw_recursive(NVGcontext* vg)
{
	if (!visible)
		return;

	draw(vg);
	for (auto const& child : children) {
		child->draw_recursive(vg);
	}
}

bool element::is_focused() const
{
	return context->input_manager.focused_element == this;
}