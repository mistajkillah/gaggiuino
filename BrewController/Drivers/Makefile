# Define the C++ compiler (Clang)
CXX = clang++

# Compiler flags for C++
CXXFLAGS = -Wall -O0 -ggdb3 -fPIC

# Source files
CXX_SRCS = AverageTempSensor.cpp MAX6675_TempSensor.cpp SmoothTempSensor.cpp Spi.cpp spidev_lib++.cc I2cBusLinux.cpp SpiLinux.cpp ADS1X15.cpp Arduino.cpp PSM.cpp I2cBus.cpp

# Object files
CXX_OBJS = $(CXX_SRCS:.cpp=.o)
CXX_OBJS := $(CXX_OBJS:.cc=.o)

# Header files
HEADERS = AverageTempSensor.h MAX6675_TempSensor.h I2cBus.h SmoothTempSensor.h Spi.h spidev_lib++.h I2cBusLinux.h GenericDrivers.h TempSensor.h PSM.h I2cBus.h

# Library name
LIB = libGenericDrivers.a

# Rule to create object files from cpp files
%.o: %.cpp $(HEADERS)
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Rule to create object files from cc files
%.o: %.cc $(HEADERS)
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Rule to create object files from c files (compiled as C++)
%.o: %.c $(HEADERS)
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Rule to create the static library
$(LIB): $(CXX_OBJS)
	ar rcs $@ $^

# Phony targets
.PHONY: all clean

# Default target
all: $(LIB)

# Clean target
clean:
	rm -f $(CXX_OBJS) $(LIB)
