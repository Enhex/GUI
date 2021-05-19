#pragma once

#include "modal.h"

#include "file_dialog.h"

/*
file_dialog with built-in modal
*/
struct file_dialog_modal : modal
{
	inline static constexpr auto element_name{ "file_dialog_modal" };
	std::string get_element_name() override { return element_name; }

	std::type_info const& type_info() const override { return typeid(file_dialog_modal); }

	file_dialog& file_di;

	file_dialog_modal();
};