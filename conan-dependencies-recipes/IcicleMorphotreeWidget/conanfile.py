from conans import ConanFile, CMake, tools

class IcicleMorphotreeWidget(ConanFile):
  name = "IcicleMorphotreeWidget"
  version = "0.0.1"
  license = "<Put the package license here>"
  description = "Interactive icicle morphotree widget Qt"
  settings = "os", "compiler", "build_type", "arch"
  options = {"shared": [True, False], "compiler": ["gcc", "clang"]}
  default_options = {"shared": False, "compiler": "gcc"}
  generators = "cmake", "cmake_find_package"
  requires = ("morphotree/0.0.1@dennisjosesilva/dev")

  def source(self):    
    git = tools.Git(folder=".")
    git.clone("https://github.com/dennisjosesilva/IcicleMorphotreeWidget.git", "main")

  def export_sources(self):
    self.copy("*.cpp", dst="src", src="IcicleMorphotreeWidget/src")
    self.copy("CMakeLists.txt", dst="src", src="IcicleMorphotreeWidget/src")
    self.copy("*.hpp", dst="include", src="IcicleMorphotreeWidget/include")

  def build(self):
    cmake = CMake(self)
    cmake.configure(source_folder="src")
    cmake.build()

  def package(self):
    self.copy("*.hpp", dst="include", src="include")
    self.copy("*.lib", dst="lib", keep_path=False)
    self.copy("*.dll", dst="bin", keep_path=False)
    self.copy("*.dylib", dst="lib", keep_path=False)
    self.copy("*.so", dst="lib", keep_path=False)
    self.copy("*.a", dst="lib", keep_path=False)

  def package_info(self):
    self.cpp_info.libs = ["IcicleMorphotreeWidget"]