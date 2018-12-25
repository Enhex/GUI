#include "serialization.h"


namespace deco
{
	void read(EntryObject const& entry, NVGcolor& value)
	{
		auto read_num = [&](uint_fast8_t i)
		{
			auto const& content = entry.entries[i].content;
			std::from_chars(content.data(), content.data() + content.size(), value.rgba[i]);
		};
		read_num(0);
		read_num(1);
		read_num(2);
		read_num(3);
	}

	void read(EntryObject const& entry, vector2& value)
	{
		auto read_num = [&](uint_fast8_t i)
		{
			auto const& content = entry.entries[i].content;
			std::from_chars(content.data(), content.data() + content.size(), value.a[i]);
		};
		read_num(0);
		read_num(1);
	}

	void read(deco::EntryObject& entry, rectangle& value)
	{
		/*
		- need to dynamically choose type based on property name.
			- start with a simple if-else
				- also allows overriding
		*/
		for (auto const& entry : entry.entries)
		{
			//TODO can early exit if all properties were consumed? what if the same property appears twice?
			auto const& name = entry.content;

			//TODO erase entries when they're consumed so derived classes' serialization doesn't have to needlessly iterate over them again
			if (name == "position")
				read(entry, value.position);
			else if (name == "size")
				read(entry, value.size);
		}
	}

	void read(deco::EntryObject & entry, gui::layout::box & value)
	{
		auto const& content = entry.entries[0].content;
		std::from_chars(content.data(), content.data() + content.size(), reinterpret_cast<int&>(value.orient));
	}

	void read(deco::EntryObject& entry, element& value)
	{
		read(entry, static_cast<rectangle&>(value));

		for (auto& entry : entry.entries)
		{
			auto const& name = entry.content;

			if (name == "style") {
				value.style = entry.entries[0].content;
			}
			else if (name == "min_size")
				read(entry, value.min_size);
			else if (name == "child_layout") {
				auto& child_layout = value.create_layout<gui::layout::box>();//TODO hack for testing, need to handle different layout types
				read(entry, child_layout);
			}
			else if (name == "children")
			{
				for(auto& child : entry.entries)
				{
					if (child.content == "element") {
						auto& child_element = emplace_back_derived<element>(value.children);
						read(child, child_element);
					}
					else if (child.content == "panel") {
						auto& child_element = emplace_back_derived<panel>(value.children);
						read(child, child_element);
					}
					else if (child.content == "text") {
						auto& child_element = emplace_back_derived<text>(value.children);
						read(child, child_element);
					}
				}
			}

			// apply style before layouting (needed for things like font size)
			value.apply_style();

			// perform layout after children are created
			if(value.child_layout)
				value.child_layout->perform();
		}
	}


	void read(deco::EntryObject& entry, panel& value)
	{
		read(entry, static_cast<element&>(value));

		for (auto const& entry : entry.entries)
		{
			auto const& name = entry.content;
		}
	}

	void read(deco::EntryObject & entry, text & value)
	{
		read(entry, static_cast<element&>(value));

		for (auto const& entry : entry.entries)
		{
			auto const& name = entry.content;

			if (name == "string") {
				for (auto const& line : entry.entries) {
					value.str += line.content;
					value.str += '\n';
				}
			}
		}
	}
}