# Main Makefile for gpio_control

# Build target: native or rpi (default is native)
TARGET ?= native

# Toolchain and compiler selection
ifeq ($(TARGET),rpi)
    CXX = arm-linux-gnueabihf-g++
    CC = arm-linux-gnueabihf-gcc
else
    CXX = g++
    CC = gcc
endif

# Compiler and linker configurations
CXXFLAGS = -I./Drivers -I./Drivers/pigpio/include -Wall -ggdb3 -O0 -I/usr/include/jsoncpp
LDFLAGS = -L./Drivers -L./Drivers/pigpio/lib -lGenericDrivers -lpigpio -lrt -lsqlite3 -lpthread -ljsoncpp

# Output binary
OUTPUT = brewController

# Source files
SRC = main.cpp SystemFSM.cpp BrewHW.cpp Pump.cpp BrewDB.cpp Profile.cpp measurements.cpp SimpleKalmanFilter.cpp movingAvg.cpp Rpc.cpp
OBJ = $(SRC:.cpp=.o)

# Default target
all: drivers $(OUTPUT)

# Rule for building the target binary
$(OUTPUT): $(OBJ)
	$(CXX) $(OBJ) -o $@ $(LDFLAGS)

# Rule for building object files
%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Rule for building Drivers library
drivers:
	$(MAKE) -C Drivers TARGET=$(TARGET)

# Native build target
native:
	$(MAKE) TARGET=native

# RPi build target
rpi:
	$(MAKE) TARGET=rpi

# Clean rule
clean:
	rm -f $(OBJ) $(OUTPUT)
	$(MAKE) -C Drivers clean
