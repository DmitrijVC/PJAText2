#pragma once

#include "type_aliases.h"


/**
 * @brief Functional structure for the Engine to work on.
 * The data is shared with all the Commands
 */
struct Operations {
	String file_in;
	String file_out;

	String source;

	bool is_panicked = false;
};
