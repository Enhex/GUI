#pragma once

#include <map>
#include <nanovg.h>
#include <typeinfo>
#include <charconv>

#include "../gui/gui.h"
#include "../layout/element.h"

#include <deco/Deco.h>
#include <deco/types/arithmetic.h>

static std::map<std::string, std::type_info const&> style_properties{
	{"font", typeid(int)},
	{"font_size", typeid(float)},
	{"color", typeid(NVGcolor)}
};


namespace deco
{
	template<typename Stream>
	void serialize(Stream& stream, NVGcolor& value)
	{
		serialize(stream, value.r);
		serialize(stream, value.g);
		serialize(stream, value.b);
		serialize(stream, value.a);
	}

	void read(EntryObject const& entry, NVGcolor& value);

	template<typename Stream>
	void write(Stream& stream, style::style_st& value)
	{
		for (auto&[name, value] : value)
		{
			// checking typeid instead of property name means less duplicate casts (in case multiple properties use the same type)
			auto iter = style_properties.find(name);
			if (iter == style_properties.end())
				throw std::logic_error("property name not found");

			auto& proerty_typeid = iter->second;

			if (proerty_typeid == typeid(int)) {
				serialize(stream, make_list(name, std::any_cast<int&>(value)));
			}
			else if (proerty_typeid == typeid(float)) {
				serialize(stream, make_list(name, std::any_cast<float&>(value)));
			}
			else if (proerty_typeid == typeid(NVGcolor)) {
				serialize(stream, make_list(name, std::any_cast<NVGcolor&>(value)));
			}
		}
	}

	template<typename Stream>
	void read(Stream& stream, style::style_st& value)
	{
		std::string name;
		std::any property_value;

		while (!stream.peek_list_end())
		{
			serialize(stream, begin_list(name));

			auto iter = style_properties.find(name);
			if (iter == style_properties.end())
				throw std::logic_error("property name not found");

			auto& proerty_typeid = iter->second;

			if (proerty_typeid == typeid(int)) {
				int input;
				serialize(stream, input);
				value.emplace(name, input);
			}
			else if (proerty_typeid == typeid(float)) {
				float input;
				serialize(stream, input);
				value.emplace(name, input);
			}
			else if (proerty_typeid == typeid(NVGcolor)) {
				NVGcolor input;
				serialize(stream, input);
				value.emplace(name, input);
			}
			serialize(stream, end_list);
		}
	}


	template<typename Stream>
	void serialize(Stream& stream, vector2& value)
	{
		serialize(stream, value.a[0]);
		serialize(stream, value.a[1]);
	}

	void read(EntryObject const& entry, vector2& value);


	template<typename Stream>
	void read_unordered_properties(Stream& stream, std::vector<std::pair<std::string, std::function<void(Stream&)>>> names_funcs)
	{
		std::string list_name;

		while (!stream.peek_list_end())
		{
			serialize(stream, begin_list(list_name));

			for (auto&[name, func] : names_funcs)
			{
				if (list_name == name)
					func(stream);
			}

			serialize(stream, end_list);
		}
	}


	template<typename Stream>
	void write(Stream& stream, rectangle& value)
	{
		if(value.position != rectangle().position)
			serialize(stream, make_list("position", value.position));
		if (value.size != rectangle().size)
			serialize(stream, make_list("size", value.size));
	}

	template<typename Stream>
	void read(Stream& stream, rectangle& value)
	{
		/*
		- need to dynamically choose type based on property name.
			- start with a simple if-else
				- also allows overriding
		*/
		std::string name;

		while (!stream.peek_list_end())
		{
			serialize(stream, begin_list(name));

			if(name == "position")
				serialize(stream, value.position);
			else if(name == "size")
				serialize(stream, value.size);

			serialize(stream, end_list);
		}
	}

	void read(deco::EntryObject& entry, rectangle& value);


	template<typename Stream>
	void serialize(Stream& stream, gui::layout::box& value)
	{
		//TODO serialize orientation as a string. would be useful to have some enum-to-string utility
		serialize(stream, make_list("orient", reinterpret_cast<int&>(value.orient)));
	}

	void read(deco::EntryObject& entry, gui::layout::box& value);


	// elements
	template<typename Stream>
	void write(Stream& stream, element& value)
	{
		serialize(stream, static_cast<rectangle&>(value));

		if (value.min_size != element().min_size) // if not default value
			serialize(stream, make_list("min_size", value.min_size));
		
		if(value.child_layout)
			serialize(stream, make_list("child_layout", /*TODO temporary hack for testing:*/static_cast<gui::layout::box&>(*value.child_layout)));

		if (!value.children.empty())
		{
			serialize(stream, begin_list("children"));
			
			for (auto& child : value.children)
			{
				//TODO need to cast to the derived class
				if (child->type_info() == typeid(element)) {
					serialize(stream, make_list("element", static_cast<element&>(*child)));
				}
				else if (child->type_info() == typeid(panel)) {
					serialize(stream, make_list("panel", static_cast<panel&>(*child)));
				}
				else if (child->type_info() == typeid(text)) {
					serialize(stream, make_list("text", static_cast<text&>(*child)));
				}
			}

			serialize(stream, end_list);
		}
	}

	void read(deco::EntryObject& entry, element& value);


	template<typename Stream>
	void serialize(Stream& stream, panel& value)
	{
		serialize(stream, static_cast<element&>(value));
		serialize(stream, make_list("color", value.color));
	}

	void read(deco::EntryObject& entry, panel& value);



	template<typename Stream>
	void serialize(Stream& stream, text& value)
	{
		if(value.position != text().position)
			serialize(stream, make_list("position", value.position));
		if (value.str != text().str)
			serialize(stream, make_list("string", value.str));

		serialize(stream, make_list("font", value.font));//TODO convert font ID to name? directly store font name?
		serialize(stream, make_list("font size", value.font_size));
		serialize(stream, make_list("color", value.color));
	}

	void read(deco::EntryObject& entry, text& value);
}