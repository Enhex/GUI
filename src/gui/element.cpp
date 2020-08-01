#include "element.h"


std::unordered_map<std::string, element&> element::id_to_element;

element::~element()
{
	id_to_element.erase(id);
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

// get absolute position. regular `position` variable being relative to the parent
vector2 element::get_position() {
	return parent != nullptr ? parent->get_position() + position : position;
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
	draw(vg);
	for (auto const& child : children)
		child->draw_recursive(vg);
}