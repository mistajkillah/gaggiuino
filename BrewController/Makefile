# Main Makefile for gpio_control

# Default target (native build)
TARGET ?= native

# Compiler and linker configurations for native build
CXX_NATIVE = g++
CXXFLAGS_NATIVE = -I./Drivers -Wall -ggdb3 -O0 -I/usr/include/jsoncpp
LDFLAGS_NATIVE = -L./Drivers -lGenericDrivers -lpigpio -lrt -lsqlite3 -lpthread -ljsoncpp

# Compiler and linker configurations for Raspberry Pi Zero 2 cross-compilation
CXX_RPI = arm-linux-gnueabihf-g++
CXXFLAGS_RPI = -I./Drivers -Wall -O2 -I/usr/include/jsoncpp
LDFLAGS_RPI = -L./Drivers -lGenericDrivers -lpigpio -lrt -lsqlite3 -lpthread -ljsoncpp

# Compiler and linker configurations based on target
ifeq ($(TARGET),rpi)
    CXX = $(CXX_RPI)
    CXXFLAGS = $(CXXFLAGS_RPI)
    LDFLAGS = $(LDFLAGS_RPI)
else
    CXX = $(CXX_NATIVE)
    CXXFLAGS = $(CXXFLAGS_NATIVE)
    LDFLAGS = $(LDFLAGS_NATIVE)
endif

# Output binary name
BINARY = brewController

# Source files
SRC = main.cpp BrewHW.cpp BrewDB.cpp Profile.cpp measurements.cpp SimpleKalmanFilter.cpp movingAvg.cpp Rpc.cpp
OBJ = $(SRC:.cpp=.o)

# Default target
all: drivers $(BINARY)

# Rule for making the actual binary
$(BINARY): $(OBJ)
	$(CXX) $(OBJ) -o $@ $(LDFLAGS)

# Rule for compiling object files
%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Phony targets for clean and driver library build
.PHONY: clean drivers native rpi

# Rule to make the drivers library
drivers:
	$(MAKE) -C Drivers TARGET=$(TARGET)

# Clean rule
clean:
	$(RM) $(OBJ) $(BINARY)
	$(MAKE) -C Drivers clean

# Target for native build
native:
	$(MAKE) TARGET=native

# Target for Raspberry Pi build
rpi:
	$(MAKE) TARGET=rpi
