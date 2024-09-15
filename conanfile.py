from conan import ConanFile
from conan.tools.cmake import CMakeToolchain, CMake, cmake_layout, CMakeDeps
from conan.tools.scm import Git

import re

class kleardb(ConanFile):
    name = "kleardb"

    # Optional metadata
    author = "Joseph Holten & Max Hirsch (joseph@holten.com, max.levin.hirsch@icloud.com)"
    description = "A SQL database implementation"

    requires = "fmt/[>=11]", "gtest/[>=1.15]"

    settings = "os", "compiler", "build_type", "arch"
    options = {"run_tests": [True, False]}
    default_options = {"run_tests": True}

    def set_version(self):
        git = Git(self)
        if self.version:
            return

        if git.is_dirty():
            self.output.warning("working tree is not clean")

        descr = git.run("describe --tags")
        regex = r"^(\d+\.\d+\.\d+(?:-pre)?)(?:-(\d+)-((?:\d|\w)+))?$"
        groups = re.findall(regex, descr)
        if len(groups) != 1:
            self.output.error(f"unknown tag format '{descr}'")
            exit(1)
        else:
            group = groups[0]
            tag, commits, h = group
            if commits == "" and h == "":
                self.version = descr
            else:
                if "-pre" not in tag:
                    self.output.error(f"commit {descr} is after tag {tag} but has no '-pre' in current tag")
                    exit(1)
                version = f"{tag}.{commits}+{h}"
                self.version = version
        self.output.highlight(f"version = {self.version}")

    def layout(self):
        cmake_layout(self)

    def generate(self):
        deps = CMakeDeps(self)
        deps.generate()

        tc = CMakeToolchain(self)
        tc.generate()

    def build(self):
        cmake = CMake(self)
        cmake.configure()
        cmake.build()

        if self.options.run_tests:
            cmake.test()
