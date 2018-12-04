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
		serialize(stream, make_list("position", value.position));
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

	// elements
	template<typename Stream>
	void serialize(Stream& stream, element& value)
	{
		serialize(stream, static_cast<rectangle&>(value));
		serialize(stream, make_list("min_size", value.min_size));
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
		serialize(stream, make_list("position", value.position));
		serialize(stream, make_list("string", value.str));
		serialize(stream, make_list("font", value.font));//TODO convert font ID to name? directly store font name?
		serialize(stream, make_list("font size", value.font_size));
		serialize(stream, make_list("color", value.color));
	}

	void read(deco::EntryObject& entry, text& value);
}