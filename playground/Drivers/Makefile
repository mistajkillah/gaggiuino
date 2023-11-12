# Define the C++ compiler
CXX = g++

# Compiler flags for C++
CXXFLAGS = -Wall -O0 -ggdb3

# Source files
CXX_SRCS =  AverageTempSensor.cpp MAX6675_TempSensor.cpp SmoothTempSensor.cpp Spi.cpp spidev_lib++.cc I2cBusLinux.cpp SpiLinux.cpp ADS1X15.cpp Arduino.cpp
CW_SRCS = CWrapper/I2cBusLinuxWrapper.cpp  CWrapper/ADS1X15Wrapper.cpp CWrapper/PSMWrapper.cpp CWrapper/SmoothTempSensorWrapper.cpp CWrapper/SpiLinuxWrapper.cpp CWrapper/MAX6675_TempSensorWrapper.cpp
CW_SRCS += CWrapper/TempSensorWrapper.cpp CWrapper/AverageTempSensorWrapper.cpp

# Object files
CXX_OBJS = $(CXX_SRCS:.cpp=.o)
CXX_OBJS := $(CXX_OBJS:.cc=.o)
CW_OBJS = $(CW_SRCS:.cpp=.o)

# Header files
HEADERS = AverageTempSensor.h MAX6675_TempSensor.h I2cBus.h SmoothTempSensor.h Spi.h spidev_lib++.h I2cBusLinux.h GenergicDrivers.h TempSensor.h
HEADERS+= CWrapper/I2cBusLinuxWrapper.h CWrapper/ADS1X15Wrapper.h CWrapper/PSMWrapper.h CWrapper/SmoothTempSensorWrapper.h CWrapper/SpiLinuxWrapper.h CWrapper/MAX6675_TempSensorWrapper.h
HEADERS += CWrapper/TempSensorWrapper.h CWrapper/AverageTempSensorWrapper.h
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
$(LIB): $(CXX_OBJS) $(CW_OBJS)
	ar rcs $@ $(CXX_OBJS) $(CW_OBJS)

# Phony targets
.PHONY: all clean

# Default target
all: $(LIB)

# Clean target
clean:
	rm -f $(CXX_OBJS) $(CW_OBJS) $(LIB)
