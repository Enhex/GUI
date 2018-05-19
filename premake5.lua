newoption {
   trigger     = "location",
   value       = "./",
   description = "Where to generate the project.",
}

if not _OPTIONS["location"] then
   _OPTIONS["location"] = "./"
end

include(_OPTIONS["location"] .. "conanpremake.lua")

workspace("GUI")
	location(_OPTIONS["location"])
	configurations { conan_build_type }
	architecture(conan_arch)

	project("GUI")
		kind "StaticLib"
		language "C++"
		cppdialect "C++17"
		targetdir = "bin/%{cfg.buildcfg}"

		files{
			"src/**",
		}

		includedirs{
			conan_includedirs
		}
		libdirs{conan_libdirs}
		links{conan_libs}
		defines{conan_cppdefines, "_SILENCE_ALL_CXX17_DEPRECATION_WARNINGS"}
		bindirs{conan_bindirs}

		filter "configurations:Debug"
			defines { "DEBUG" }
			symbols "On"

		filter "configurations:Release"
			defines { "NDEBUG" }
			optimize "On"