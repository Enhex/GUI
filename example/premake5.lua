newoption {
   trigger     = "location",
   value       = "./",
   description = "Where to generate the project.",
}

if not _OPTIONS["location"] then
   _OPTIONS["location"] = "./"
end

newoption {
	trigger     = "gui-path",
	value       = "./",
	description = "Where the GUI project is located.",
}

newoption {
	trigger     = "mingw",
	description = "MinGW is being used to cross compile.",
}

-- include GUI lib's dependencies
include(_OPTIONS["gui-path"] .. "/conandeps.premake5.lua")

workspace("GUI example")
	location(_OPTIONS["location"])
	conan_setup()
	startproject "gui_example"

	project("gui_example")
		kind "ConsoleApp"
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

		includedirs{
			"../src"
		}
		links{"GUI"}

		defines{"_SILENCE_ALL_CXX17_DEPRECATION_WARNINGS"}

		filter "configurations:Debug"
			defines { "DEBUG" }
			symbols "On"

		filter "configurations:Release"
			defines { "NDEBUG" }
			optimize "On"
			linktimeoptimization "On"
			buildoptions{"-fdata-sections -ffunction-sections"} -- needed for -gc-sections
			linkoptions{"-s -Wl,--gc-sections -Wl,--as-needed"}

	externalproject "GUI"
		print(_OPTIONS["gui-path"])
		location(_OPTIONS["gui-path"])
		kind "StaticLib"
		language "C++"