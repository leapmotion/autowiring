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
        # For Visual Studio builds, ignore build_type since we want to create both debug and release binaries
        if self.settings.compiler == "Visual Studio":
            self.settings.remove("build_type")

        self.output.info(self.version)
        
        if not os.path.exists(self.build_dir):
            os.makedirs(self.build_dir)
        
        cmake = CMake(self.settings)
        args = ['-DCMAKE_INSTALL_PREFIX="%s"' % self.package_folder]
        self.run('cd %s && cmake %s %s %s' % (self.build_dir, self.conanfile_directory, cmake.command_line, ' '.join(args)))
        if hasattr(self.settings, "build_type"):
            self.run("cd %s && cmake --build . --target install %s" % (self.build_dir, cmake.build_config))
        else:
            self.run("cd %s && cmake --build . --target install --config Release" % (self.build_dir))
            self.run("cd %s && cmake --build . --target install --config Debug" % (self.build_dir))

    def package(self):
        self.output.info(self.package_folder)

    def package_info(self):
        self.cpp_info.libs = ["Autoboost64.lib", "AutoNet64.lib", "AutoTesting64.lib", "Autowiring64.lib"]
