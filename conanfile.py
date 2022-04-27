from conans import ConanFile

# automatically choose Premake generator
def run_premake(self):
	if "Visual Studio" in self.settings.compiler:
		_visuals = {'8': '2005',
					'9': '2008',
					'10': '2010',
					'11': '2012',
					'12': '2013',
					'14': '2015',
					'15': '2017',
					'16': '2019',
					'17': '2022'}
		premake_command = "premake5 vs%s" % _visuals.get(str(self.settings.compiler.version), "UnknownVersion %s" % str(self.settings.compiler.version))
		self.run(premake_command)
	else:
		self.run("premake5 gmake2")

class GuiConan(ConanFile):
	name = "gui"
	version = "master"
	license = "<Put the package license here>"
	url = "<Package recipe repository url here, for issues about the package>"
	description = "<Description of Gui here>"
	settings = "os", "compiler", "build_type", "arch"
	options = {"shared": [True, False]}
	default_options = "shared=False", "boost:without_stacktrace=True"
	generators = "premake"
	exports = "premake5.lua"
	exports_sources = "src/*"

	requires = (
		"boost/1.78.0",
		"glfw/3.3.6",
		"glad/0.1.34",
		"nanovg/master@enhex/stable",
		"deco/master@enhex/stable"
	)

	def build(self):
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
