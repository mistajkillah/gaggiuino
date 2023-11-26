# Define the C++ compiler (Clang)
CXX = clang++

# Compiler flags for C++
CXXFLAGS = -Wall -O0 -ggdb3 -shared -fPIC

# Source files
CXX_SRCS = AverageTempSensor.cpp MAX6675_TempSensor.cpp SmoothTempSensor.cpp Spi.cpp spidev_lib++.cc I2cBusLinux.cpp SpiLinux.cpp ADS1X15.cpp Arduino.cpp PSM.cpp I2cBus.cpp
CW_SRCS = CWrapper/I2cBusLinuxWrapper.cpp CWrapper/ADS1X15Wrapper.cpp CWrapper/PSMWrapper.cpp CWrapper/SmoothTempSensorWrapper.cpp CWrapper/SpiLinuxWrapper.cpp CWrapper/MAX6675_TempSensorWrapper.cpp
CW_SRCS += CWrapper/TempSensorWrapper.cpp CWrapper/AverageTempSensorWrapper.cpp

# Object files
CXX_OBJS = $(CXX_SRCS:.cpp=.o)
CXX_OBJS := $(CXX_OBJS:.cc=.o)
CW_OBJS = $(CW_SRCS:.cpp=.o)

# Header files
HEADERS = AverageTempSensor.h MAX6675_TempSensor.h I2cBus.h SmoothTempSensor.h Spi.h spidev_lib++.h I2cBusLinux.h GenericDrivers.h TempSensor.h PSM.h I2cBus.h
HEADERS += CWrapper/I2cBusLinuxWrapper.h CWrapper/ADS1X15Wrapper.h CWrapper/PSMWrapper.h CWrapper/SmoothTempSensorWrapper.h CWrapper/SpiLinuxWrapper.h CWrapper/MAX6675_TempSensorWrapper.h
HEADERS += CWrapper/TempSensorWrapper.h CWrapper/AverageTempSensorWrapper.h

# Library name
LIB = libGenericDrivers.so

# Rust Bindings generation settings
RUST_BINDINGS_DIR = ./src/generated
RUST_BINDINGS_SRC = $(RUST_BINDINGS_DIR)/lib.rs
RUST_BINDINGS_BUILD_DIR = target/bindings

# Rule to create object files from cpp files
%.o: %.cpp $(HEADERS)
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Rule to create object files from cc files
%.o: %.cc $(HEADERS)
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Rule to create object files from c files (compiled as C++)
%.o: %.c $(HEADERS)
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Rule to create the shared object
$(LIB): $(CXX_OBJS) $(CW_OBJS)
	$(CXX) $(CXXFLAGS) -o $@ $^

# Phony targets
.PHONY: all clean

# Default target
all: $(LIB)

# Clean target
clean:
	rm -f $(CXX_OBJS) $(CW_OBJS) $(LIB)

# Create the 'generated' directory
create_generated_dir:
	mkdir -p $(RUST_BINDINGS_DIR)

# Rust Bindings generation
generate_bindings: create_generated_dir
	$(foreach header,$(filter CWrapper/%.h,$(HEADERS)),bindgen --output $(RUST_BINDINGS_DIR)/$(notdir $(header:.h=.rs)) $(header) -- $(addprefix -I, $(RUST_BINDINGS_BUILD_DIR)) $(addprefix -I, CWrapper);)

# Rust Build
rust_build: generate_bindings
	cargo build

# Rust Clean
rust_clean:
	cargo clean

# Build everything
build: all rust_build

# Clean everything
clean_all: clean rust_clean
