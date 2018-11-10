#pragma once

#include <map>
#include <nanovg.h>
#include <typeinfo>

#include "../gui/element.h"
#include "../layout/element.h"


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

	template<typename Stream>
	void serialize(Stream& stream, rectangle& value)
	{
		serialize(stream, make_list("position", value.position));
		serialize(stream, make_list("size", value.size));
	}

	// elements
	template<typename Stream>
	void serialize(Stream& stream, element& value)
	{
		//TODO should serialize members as optional, with default value
		serialize(stream, static_cast<rectangle&>(value));
		serialize(stream, make_list("min_size", value.min_size));
	}

	template<typename Stream>
	void serialize(Stream& stream, panel& value)
	{
		serialize(stream, static_cast<element&>(value));
		serialize(stream, value.color);
	}
}