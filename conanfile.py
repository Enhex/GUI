from conans import ConanFile, CMake


class GuiConan(ConanFile):
	name = "gui"
	version = "master"
	license = "<Put the package license here>"
	url = "<Package recipe repository url here, for issues about the package>"
	description = "<Description of Gui here>"
	settings = "os", "compiler", "build_type", "arch"
	options = {"shared": [True, False]}
	default_options = "shared=False"
	generators = "premake"
	exports = "premake5.lua"
	exports_sources = "src/*"
	
	requires = (
		"boost_qvm/1.67.0@bincrafters/stable",
		"glfw/3.2.1@bincrafters/stable",
		"glad/0.1.24@bincrafters/stable",
		"nanovg/master@enhex/stable",
		"Deco/master@enhex/stable",
		"premake_generator/master@enhex/stable"
	)
	
	def build(self):
		from premake import run_premake
		run_premake(self)
		self.run('build')

	def package(self):
		self.copy("*.h", dst="include", src="src")
		self.copy("*.lib", dst="lib", keep_path=False)
		self.copy("*.dll", dst="bin", keep_path=False)
		self.copy("*.dylib*", dst="lib", keep_path=False)
		self.copy("*.so", dst="lib", keep_path=False)
		self.copy("*.a", dst="lib", keep_path=False)

	def package_info(self):
		self.cpp_info.libs = ["GUI"]
