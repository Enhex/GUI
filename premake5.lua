newoption {
	trigger     = "location",
	value       = "./",
	description = "Where to generate the project.",
}

if not _OPTIONS["location"] then
	_OPTIONS["location"] = "./"
end

newoption {
	trigger     = "mingw",
	description = "MinGW is being used to cross compile.",
}

include(_OPTIONS["location"] .. "conandeps.premake5.lua")

workspace("GUI")
	location(_OPTIONS["location"])
	conan_setup()

	project("GUI")
		kind "StaticLib"
		language "C++"
		cppdialect "C++17"
		targetdir = "bin/%{cfg.buildcfg}"

		files{
			"src/**",
		}

		-- needed to cross compile
		if _OPTIONS["mingw"] then
			libdirs {"/usr/x86_64-w64-mingw32/lib"}
			linkoptions{"-static"}
		end

		defines{"_SILENCE_ALL_CXX17_DEPRECATION_WARNINGS"}

		pchsource "src/framework/precompiled.cpp"

		filter "toolset:gcc"
			pchheader "src/framework/precompiled.h"
			forceincludes  { "src/framework/precompiled.h" }

		filter "toolset:not gcc"
			pchheader "precompiled.h"
			forceincludes { "precompiled.h" }

		filter "configurations:Debug"
			defines { "DEBUG" }
			symbols "On"

		filter "configurations:Release"
			defines { "NDEBUG" }
			optimize "On"
			buildoptions{"-fdata-sections -ffunction-sections"} -- needed for -gc-sections
			linkoptions{"-s -Wl,--gc-sections -Wl,--as-needed"}