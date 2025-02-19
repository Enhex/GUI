import os
from conan import ConanFile
from conan.tools.files import copy

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
	settings = "os", "compiler", "build_type", "arch"
	options = {"shared": [True, False], "fPIC": [True, False]}
	default_options = {"shared": False, "fPIC": True, "boost/*:without_stacktrace": True}
	generators = "PremakeDeps"
	exports_sources = "premake5.lua", "src/*"

	requires = (
		"glfw/3.4",
		"glad/0.1.36",
		"nanovg/master",
		"deco/master"
	)

	def requirements(self):
		self.requires("boost/1.86.0", transitive_headers=True)

	def build(self):
		run_premake(self)
		self.run('build')

	def package(self):
		copy(self, "*.h", os.path.join(self.source_folder, "src"), os.path.join(self.package_folder, "include"))
		copy(self, "*.a", self.build_folder, os.path.join(self.package_folder, "lib"), keep_path=False)
		copy(self, "*.so", self.build_folder, os.path.join(self.package_folder, "lib"), keep_path=False)
		copy(self, "*.lib", self.build_folder, os.path.join(self.package_folder, "lib"), keep_path=False)
		copy(self, "*.dll", self.build_folder, os.path.join(self.package_folder, "bin"), keep_path=False)
		copy(self, "*.dylib", self.build_folder, os.path.join(self.package_folder, "lib"), keep_path=False)

	def package_info(self):
		self.cpp_info.libs = ["GUI"]
