from conans import ConanFile, CMake


class KeaOpenCVViewer(ConanFile):
    name = "kea_opencv_viewer"
    version = "1.0.0"
    license = "Private"
    url = "https://www.chronoptics.com"
    description = "Example of using the Chronoptics ToF library"
    settings = "os", "compiler", "build_type", "arch"

    requires = [
        "tof/3.0.0",
        "opencv/4.5.0",
    ]
    build_requires = ["cmake_helpers/1.0.1"]
    generators = "cmake_find_package", "cmake_paths"
    exports_sources = [
        "src/*",
        "CMakeLists.txt",
    ]

    def configure(self):
        self.options["catch2"].with_main = True
        if not self.settings.os == "Windows":
            self.options["opencv"].with_gtk = False

    def imports(self):
        self.copy("*.dll", "bin", "bin")
        self.copy("*.dylib", "lib", "lib")

    def _configure_cmake(self):
        cmake = CMake(self)
        cmake.configure(source_folder="")
        return cmake

    def build(self):
        cmake = self._configure_cmake()
        cmake.build()

    def package(self):
        cmake = self._configure_cmake()
        cmake.install()

    def package_info(self):
        self.cpp_info.libs = tools.collect_libs(self)
