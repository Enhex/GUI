#include "serialization.h"

namespace fs = std::filesystem;

std::filesystem::path user_config_path()
{
	fs::path path;
	auto cstr = std::getenv("XDG_CONFIG_HOME");
	if(cstr != nullptr) {
		path = cstr;
		return path;
	}

#ifdef _WIN32
	path = std::getenv("APPDATA");
#else
	path = std::getenv("HOME");
	path /= ".config";
#endif

	return path;
}