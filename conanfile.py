from conans import ConanFile, CMake, tools
import os, re, sys

class AutowiringConan(ConanFile):
    name = "autowiring"
    version = "1.0.4"
    #re.match(".*VERSION\s+([0-9|\.]*)\)", open("%s/version.cmake" % sys.argv[0]).read()).group(1)
    license = "Apache-2.0"
    author = "Leap Motion Inc. (www.leapmotion.com)"
    url = "https://github.com/leapmotion/autowiring.git"
    settings = "os", "compiler", "build_type", "arch"
    exports = "cmake-modules/*", "contrib/*", "devguide/*", "examples/*", "nuget/*", "scripts/*", "src/*", "standard/*", \
                ".travis.yml", "Autowiring.nuspec.in", "CMakeLists.txt", "CONTRIBUTORS.txt", "Doxyfile", "LICENSE.txt", "README.md", "publicDoxyfile.conf", "tests.cmake", "version.cmake"
    generators = "cmake"
    build_dir = "b"
    short_paths = True

    def build(self):
        self.output.info(self.version)
        
        if not os.path.exists(self.build_dir):
            os.makedirs(self.build_dir)
        
        cmake = CMake(self.settings)
        args = ['-DCMAKE_INSTALL_PREFIX="%s"' % self.package_folder]

        if self.settings.compiler == "Visual Studio" and self.settings.build_type == "Debug":
            if not str(self.settings.compiler.runtime).endswith("d"):
                self.settings.compiler.runtime = str(self.settings.compiler.runtime) + "d"

        self.run('cd %s && cmake %s %s %s' % (self.build_dir, self.conanfile_directory, cmake.command_line, ' '.join(args)))
        self.run("cd %s && cmake --build . %s --target install" % (self.build_dir, cmake.build_config))

    def package(self):
        self.output.info(self.package_folder)

    def package_info(self):
        self.cpp_info.libs = ["hello"]
