# Define the C++ compiler
CXX = g++

# Compiler flags
CXXFLAGS = -Wall -O0  -ggdb3

# Source files
SRCS = GenericDrivers.cpp AverageTempSensor.cpp MAX6675_TempSensor.cpp I2cBus.cpp SmoothTempSensor.cpp Spi.cpp spidev_lib++.cc I2cBusLinux.cpp SpiLinux.cpp ADS1X15.cpp

# Object files
OBJS = $(SRCS:.cpp=.o)
OBJS := $(OBJS:.cc=.o)

# Header files
HEADERS = AverageTempSensor.h MAX6675_TempSensor.h I2cBus.h SmoothTempSensor.h Spi.h spidev_lib++.h I2cBusLinux.h GenergicDrivers.h TempSensor.h

# Library name
LIB = libGenericDrivers.a

# Rule to create object files from cpp files
%.o: %.cpp $(HEADERS)
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Rule to create object files from cc files
%.o: %.cc $(HEADERS)
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Rule to create the static library
$(LIB): $(OBJS)
	ar rcs $@ $(OBJS)

# Phony targets
.PHONY: all clean

# Default target
all: $(LIB)

# Clean target
clean:
	rm -f $(OBJS) $(LIB)
