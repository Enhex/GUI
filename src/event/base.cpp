#include "../input/element.h"

namespace input
{
	namespace event
	{
		// used to track if an element deleted itself from one of its callbacks, to prevent calling the rest of its callbacks.
		// it's marked as deleted by setting the pointer to nullptr.
		element* callback_element = nullptr;

		// used to handle deleting subsription while iterating
		element* active_callback_element = nullptr;

		bool stop_callbacks = false;
	}
}