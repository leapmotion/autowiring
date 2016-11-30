from conans import ConanFile, CMake
import re
import os

class AutowiringConan(ConanFile):
    name = "Autowiring"
    settings = "os", "compiler", "build_type", "arch"
    exports = "*"
    version = re.match(".*VERSION\s+([0-9|\.]*)\)",open("version.cmake").read()).group(1)

    def build(self):
        self.output.info(self.version)
        build_dir="b"
        if not os.path.exists(build_dir):
            os.makedirs(build_dir)
        
        cmake = CMake(self.settings)
        args = ['-DCMAKE_INSTALL_PREFIX="%s"' % self.package_folder]

        self.run('cd %s && cmake %s %s %s' % (build_dir, self.conanfile_directory, cmake.command_line, ' '.join(args)))
        self.run("cd %s && cmake --build . %s" % (build_dir, cmake.build_config))

    def package(self):
        self.output.info("handled by the install step")

    def package_info(self):
        self.cpp_info.libs = ["hello"]