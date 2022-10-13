# Distributed under the OSI-approved BSD 3-Clause License.  See accompanying
# file Copyright.txt or https://cmake.org/licensing for details.

cmake_minimum_required(VERSION 3.5)

file(MAKE_DIRECTORY
  "C:/Users/Malte/esp/esp-idf/components/bootloader/subproject"
  "C:/Users/Malte/Desktop/UVMedico_SW/DockSW/build/bootloader"
  "C:/Users/Malte/Desktop/UVMedico_SW/DockSW/build/bootloader-prefix"
  "C:/Users/Malte/Desktop/UVMedico_SW/DockSW/build/bootloader-prefix/tmp"
  "C:/Users/Malte/Desktop/UVMedico_SW/DockSW/build/bootloader-prefix/src/bootloader-stamp"
  "C:/Users/Malte/Desktop/UVMedico_SW/DockSW/build/bootloader-prefix/src"
  "C:/Users/Malte/Desktop/UVMedico_SW/DockSW/build/bootloader-prefix/src/bootloader-stamp"
)

set(configSubDirs )
foreach(subDir IN LISTS configSubDirs)
    file(MAKE_DIRECTORY "C:/Users/Malte/Desktop/UVMedico_SW/DockSW/build/bootloader-prefix/src/bootloader-stamp/${subDir}")
endforeach()
