# Define the C++ compiler
CXX = g++

# Compiler flags
CXXFLAGS = -Wall -O2

# Source files
SRCS = GenericDrivers.cpp AverageThermocouple.cpp MAX6675_Thermocouple.cpp I2cBus.cpp SmoothThermocouple.cpp Spi.cpp spidev_lib++.cc I2cBusLinux.cpp

# Object files
OBJS = $(SRCS:.cpp=.o)
OBJS := $(OBJS:.cc=.o)

# Header files
HEADERS = AverageThermocouple.h MAX6675_Thermocouple.h I2cBus.h SmoothThermocouple.h Spi.h spidev_lib++.h I2cBusLinux.h GenergicDrivers.h Thermocouple.h

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