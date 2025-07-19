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
newoption {
	trigger     = "armv8",
	description = "cross compile to ARM v8.",
}

-- include GUI lib's dependencies
include(_OPTIONS["gui-path"] .. "conandeps.premake5.lua")

workspace("GUI example")
	location(_OPTIONS["location"])
	conan_setup()
	startproject "gui_example"

	project("gui_example")
		kind "ConsoleApp"
		language "C++"
		cppdialect "C++17"
		targetdir(_OPTIONS["location"] .. "bin")

		files{
			"src/**",
		}

		if _OPTIONS["armv8"] then
			libdirs {"/usr/aarch64-linux-gnu/lib"}
			linkoptions{"-static"}
		end

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
			buildoptions{"-fdata-sections -ffunction-sections"} -- needed for -gc-sections
			linkoptions{"-s -Wl,--gc-sections -Wl,--as-needed"}
			if not _OPTIONS["mingw"] then
				linktimeoptimization "On"
			end

	externalproject "GUI"
		print(_OPTIONS["gui-path"])
		location(_OPTIONS["gui-path"])
		targetdir(_OPTIONS["gui-path"] .. "bin")
		kind "StaticLib"
		language "C++"