newoption {
	trigger     = "location",
	value       = "./",
	description = "Where to generate the project.",
}

if not _OPTIONS["location"] then
	_OPTIONS["location"] = "./"
end

include(_OPTIONS["location"] .. "conanbuildinfo.premake.lua")

workspace("GUI")
	location(_OPTIONS["location"])
	conan_basic_setup()

	project("GUI")
		kind "StaticLib"
		language "C++"
		cppdialect "C++17"
		targetdir = "bin/%{cfg.buildcfg}"

		files{
			"src/**",
		}

		pchheader "precompiled.h"
		pchsource "src/framework/precompiled.cpp"
		forceincludes  { "precompiled.h" }

		defines{"_SILENCE_ALL_CXX17_DEPRECATION_WARNINGS"}

		filter "configurations:Debug"
			defines { "DEBUG" }
			symbols "On"

		filter "configurations:Release"
			defines { "NDEBUG" }
			optimize "On"