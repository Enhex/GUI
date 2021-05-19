#include "file_dialog_modal.h"

file_dialog_modal::file_dialog_modal() :
	file_di(create_child<file_dialog>())
{
	file_di.visibility_element = this;
}