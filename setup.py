import os
import re
import subprocess
import sys
from pathlib import Path
import shutil

from setuptools import Extension, setup
from setuptools.command.build_ext import build_ext

# Convert distutils Windows platform specifiers to CMake -A arguments
PLAT_TO_CMAKE = {
    "win32": "Win32",
    "win-amd64": "x64",
    "win-arm32": "ARM",
    "win-arm64": "ARM64",
}


# A CMakeExtension needs a sourcedir instead of a file list.
# The name must be the _single_ output extension from the CMake build.
# If you need multiple extensions, see scikit-build.
class CMakeExtension(Extension):
    def __init__(self, name: str, sourcedir: str = "") -> None:
        super().__init__(name, sources=[])
        self.sourcedir = os.fspath(Path(sourcedir).resolve())


class CMakeBuild(build_ext):
    def build_extension(self, ext: CMakeExtension) -> None:
        # Must be in this form due to bug in .resolve() only fixed in Python 3.10+
        ext_fullpath = Path.cwd() / self.get_ext_fullpath(ext.name)
        extdir = ext_fullpath.parent.resolve()

        debug = int(os.environ.get("DEBUG", 0)) if self.debug is None else self.debug
        cfg = "Debug" if debug else "Release"
        # CMake lets you override the generator - we need to check this.
        # Can be set with Conda-Build, for example.
        cmake_generator = os.environ.get("CMAKE_GENERATOR", "")

        # Set Python_EXECUTABLE instead if you use PYBIND11_FINDPYTHON
        # EXAMPLE_VERSION_INFO shows you how to pass a value into the C++ code
        # from Python.
        cmake_args = [
            f"-DCMAKE_LIBRARY_OUTPUT_DIRECTORY={extdir}{os.sep}",
            f"-DPYTHON_EXECUTABLE={sys.executable}",
            f"-DCMAKE_BUILD_TYPE={cfg}",  # not used on MSVC, but no harm
        ]
        build_args = []


        build_temp = Path(self.build_temp) / ext.name
        if not build_temp.exists():
            build_temp.mkdir(parents=True)


        print("Downloading vgmstream dependencies...")
        # 下载依赖
        if sys.platform == "linux" or sys.platform == "linux2":
            subprocess.run(
                ['sh', './install_dependency/linux.sh'], check=True
            )
        elif sys.platform == "darwin":
            subprocess.run(
                ['sh', './install_dependency/macos.sh'], check=True
            )
        elif sys.platform == "win32":
            cmake_args.append("-G")
            cmake_args.append('Unix Makefiles')

            source_folder = ".\\vgmstream\\ext_libs\\dll-x64"
            dll_files = [file for file in os.listdir(source_folder) if file.lower().endswith(".dll")]
            dest_folder = extdir
            if not os.path.exists(dest_folder):
                os.makedirs(dest_folder)
                print(f"Created {dest_folder}")

            # Copy each .dll file to the destination folder
            for dll_file in dll_files:
                source_path = os.path.join(source_folder, dll_file)
                dest_path = os.path.join(dest_folder, dll_file)
                shutil.copy(source_path, dest_path)
                print(f"Copied {dll_file} to {dest_folder}")
            

        print(cmake_args)
        subprocess.run(
            ["cmake", ext.sourcedir, *cmake_args], cwd=build_temp, check=True
        )
        subprocess.run(
            ["cmake", "--build", ".", "--", "-j"], cwd=build_temp, check=True
        )
 



# The information here can also be placed in setup.cfg - better separation of
# logic and declaration, and simpler if you include description/version in a file.
setup(
    name="pyvgmstream",
    version="0.0.1",
    author="Huge_Black",
    description="A vgmstream library that you can directly call from python.",
    long_description="",
    ext_modules=[CMakeExtension("pyvgmstream.libpyvgmstream")],
    cmdclass={"build_ext": CMakeBuild},
    zip_safe=False,
    python_requires=">=3.7",
    packages=['pyvgmstream']
)
