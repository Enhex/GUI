#include "file_dialog_modal.h"

file_dialog_modal::file_dialog_modal() :
	file_di(create_child<file_dialog>())
{
	file_di.visibility_element = this;

	context->input_manager.mouse_press.subscribe(&file_di, [](int key, int mods) {
		// prevent clicks on the dialog from propagating to the modal
	});
}