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

		defines{"_SILENCE_ALL_CXX17_DEPRECATION_WARNINGS"}

		pchsource "src/framework/precompiled.cpp"

		filter "toolset:gcc"
			pchheader "src/framework/precompiled.h"
			forceincludes  { "src/framework/precompiled.h" }

		filter "toolset:not gcc"
			pchheader "precompiled.h"
			forceincludes  { "precompiled.h" }

		filter "configurations:Debug"
			defines { "DEBUG" }
			symbols "On"

		filter "configurations:Release"
			defines { "NDEBUG" }
			optimize "On"
			flags{"LinkTimeOptimization"}
			buildoptions{"-fdata-sections -ffunction-sections"} -- needed for -gc-sections
			linkoptions{"-s -Wl,--gc-sections -Wl,--as-needed"}