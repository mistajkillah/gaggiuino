# gcc_toolchain.cmake
# Toolchain file for native GCC

# Specify the system
set(CMAKE_SYSTEM_NAME Linux)

# Specify the compilers
set(CMAKE_C_COMPILER gcc)
set(CMAKE_CXX_COMPILER g++)
set(CMAKE_MAKE_PROGRAM make)

# Compiler flags
set(CMAKE_C_FLAGS "-Wall -O2")
set(CMAKE_CXX_FLAGS "-Wall -O2")
