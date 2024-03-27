# Distributed under the OSI-approved BSD 3-Clause License.  See accompanying
# file Copyright.txt or https://cmake.org/licensing for details.

cmake_minimum_required(VERSION 3.5)

file(MAKE_DIRECTORY
  "E:/esp32/Espressif/frameworks/esp-idf-v5.1.2/components/bootloader/subproject"
  "E:/esp32project/SolarControl/build/bootloader"
  "E:/esp32project/SolarControl/build/bootloader-prefix"
  "E:/esp32project/SolarControl/build/bootloader-prefix/tmp"
  "E:/esp32project/SolarControl/build/bootloader-prefix/src/bootloader-stamp"
  "E:/esp32project/SolarControl/build/bootloader-prefix/src"
  "E:/esp32project/SolarControl/build/bootloader-prefix/src/bootloader-stamp"
)

set(configSubDirs )
foreach(subDir IN LISTS configSubDirs)
    file(MAKE_DIRECTORY "E:/esp32project/SolarControl/build/bootloader-prefix/src/bootloader-stamp/${subDir}")
endforeach()
if(cfgdir)
  file(MAKE_DIRECTORY "E:/esp32project/SolarControl/build/bootloader-prefix/src/bootloader-stamp${cfgdir}") # cfgdir has leading slash
endif()
