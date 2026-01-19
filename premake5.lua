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
newoption {
	trigger     = "armv8",
	description = "cross compile to ARM v8.",
}
newoption {
	trigger     = "sysroot",
	description = "sysroot's path",
}


include(_OPTIONS["location"] .. "conandeps.premake5.lua")

workspace("GUI")
	location(_OPTIONS["location"])
	conan_setup()

	project("GUI")
		kind "StaticLib"
		language "C++"
		cppdialect "C++17"
		targetdir(_OPTIONS["location"] .. "bin")

		files{
			"src/**",
		}

		includedirs{
			"src"
		}

		if _OPTIONS["armv8"] then
			libdirs {"/usr/aarch64-linux-gnu/lib"}
			buildoptions {"-Wl,-rpath-link=" .. _OPTIONS["sysroot"] .. "/usr/lib/aarch64-linux-gnu/"}
			libdirs {_OPTIONS["sysroot"] .. "/usr/lib/aarch64-linux-gnu"}
		end

		-- needed to cross compile
		if _OPTIONS["mingw"] then
			libdirs {"/usr/x86_64-w64-mingw32/lib"}
			linkoptions{"-static"}
		end

		pchsource "src/framework/precompiled.cpp"
		pchheader "src/framework/precompiled.h"
		forceincludes  { "src/framework/precompiled.h" }

		filter "configurations:Debug"
			defines { "DEBUG" }
			symbols "On"

		filter "configurations:Release"
			defines { "NDEBUG" }
			optimize "On"
			buildoptions{"-fdata-sections -ffunction-sections"} -- needed for -gc-sections
			linkoptions{"-s -Wl,--gc-sections -Wl,--as-needed"}
