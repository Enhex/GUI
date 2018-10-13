#pragma once

#include <any>
#include <memory>
#include <strong_type.h>
#include <unordered_map>

namespace style
{
	//STRONG_TYPE(any, std::any); // used for template specialization

	// property name to value
	using style_t = std::unordered_map<std::string, std::any>;

	// strong type style_t for serialization specialization - choose std::any's type according to property name
	STRONG_TYPE(style_st, style_t);

	struct manager
	{
		// map from style name to style
		std::unordered_map<std::string, style_st> styles;
	};
}