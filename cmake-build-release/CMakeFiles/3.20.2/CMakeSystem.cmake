set(CMAKE_HOST_SYSTEM "Linux-5.11.0-34-generic")
set(CMAKE_HOST_SYSTEM_NAME "Linux")
set(CMAKE_HOST_SYSTEM_VERSION "5.11.0-34-generic")
set(CMAKE_HOST_SYSTEM_PROCESSOR "x86_64")

include("/home/tanuq/dev/Arduino-CMake-Toolchain/Arduino-toolchain.cmake")

set(CMAKE_SYSTEM "Arduino")
set(CMAKE_SYSTEM_NAME "Arduino")
set(CMAKE_SYSTEM_VERSION "")
set(CMAKE_SYSTEM_PROCESSOR "")
include("/home/tanuq/dev/asao/cmake-build-release/ArduinoSystem.cmake")
set(CMAKE_CROSSCOMPILING "TRUE")

set(CMAKE_SYSTEM_LOADED 1)
