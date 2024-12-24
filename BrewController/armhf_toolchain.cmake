# armhf_toolchain.cmake
# Toolchain file for ARM HF cross-compilation

# Specify the system
set(CMAKE_SYSTEM_NAME Linux)
set(CMAKE_SYSTEM_PROCESSOR arm)

# Specify the compilers
set(CMAKE_C_COMPILER arm-linux-gnueabihf-gcc)
set(CMAKE_CXX_COMPILER arm-linux-gnueabihf-g++)
set(CMAKE_MAKE_PROGRAM make)

# Specify the sysroot (optional, adjust path as needed)
set(CMAKE_SYSROOT /usr/arm-linux-gnueabihf)

# Compiler flags
set(CMAKE_C_FLAGS "--sysroot=${CMAKE_SYSROOT} -Wall -O2")
set(CMAKE_CXX_FLAGS "--sysroot=${CMAKE_SYSROOT} -Wall -O2")

# Additional linker flags (optional)
set(CMAKE_EXE_LINKER_FLAGS "--sysroot=${CMAKE_SYSROOT}")

# Specify paths for libraries and headers
set(CMAKE_FIND_ROOT_PATH ${CMAKE_SYSROOT})
set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)
