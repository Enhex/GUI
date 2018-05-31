#pragma once

#include <any>
#include <memory>
#include <unordered_map>

namespace style
{
	// property name to value
	using style_t = std::unordered_map<std::string, std::any>;

	struct manager
	{
		// map from style name to style
		std::unordered_map<std::string, style_t> styles;
	};
}