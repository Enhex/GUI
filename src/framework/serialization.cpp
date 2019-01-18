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

	void read_element(deco::EntryObject& entry, element& parent)
	{
		auto const& name = entry.content;

		if (name == element::element_name) {
			auto& child_element = emplace_back_derived<element>(parent.children);
			read(entry, child_element);
		}
		if (name == scissor::element_name) {
			auto& child_element = emplace_back_derived<scissor>(parent.children);
			read(entry, child_element);
		}
		else if (name == panel::element_name) {
			auto& child_element = emplace_back_derived<panel>(parent.children);
			read(entry, child_element);
		}
		else if (name == text::element_name) {
			auto& child_element = emplace_back_derived<text>(parent.children);
			read(entry, child_element);
		}
		else if (name == text_edit::element_name) {
			auto& child_element = emplace_back_derived<text_edit>(parent.children);
			read(entry, child_element);
		}
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
			else if (name == "layout-box") {
				auto& child_layout = value.create_layout<gui::layout::box>();
				read(entry, child_layout);
			}
			else if (name == "expand")
			{
				auto read_bool = [&](uint_fast8_t i)
				{
					auto const& content = entry.entries[i].content;
					if (content == "0")
						value.expand[i] = false;
					else if (content == "1")
						value.expand[i] = true;
				};
				read_bool(0);
				read_bool(1);
			}
			else
			{
				read_element(entry, value);
			}

			// apply style before layouting (needed for things like font size)
			value.apply_style();

			// perform layout after children are created
			if(value.child_layout)
				value.child_layout->perform();
		}
	}

	void read(deco::EntryObject & entry, scissor & value)
	{
		read(entry, static_cast<element&>(value));
	}


	void read(deco::EntryObject& entry, panel& value)
	{
		read(entry, static_cast<element&>(value));
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


	void read(deco::EntryObject & entry, text_edit & value)
	{
		read(entry, static_cast<text&>(value));
	}
}