location_dir = "../build/"

include(location_dir .. "conanpremake.lua")

workspace("GUI")
	location(location_dir)
	configurations { conan_build_type }
	architecture(conan_arch)

	project("GUI")
		kind "ConsoleApp"
		language "C++"
		cppdialect "C++17"
		targetdir = location_dir .. "bin/%{cfg.buildcfg}"

		files{
			"src/**",
		}

		includedirs{
			conan_includedirs
		}
		libdirs{conan_libdirs}
		links{conan_libs, "OpenGL32.lib"}
		defines{conan_cppdefines, "_SILENCE_ALL_CXX17_DEPRECATION_WARNINGS"}
		bindirs{conan_bindirs}

		filter "configurations:Debug"
			defines { "DEBUG" }
			symbols "On"

		filter "configurations:Release"
			defines { "NDEBUG" }
			optimize "On"