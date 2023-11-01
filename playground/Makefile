# Main Makefile for gpio_control

# Compiler and linker configurations
CXX = g++
CXXFLAGS = -I./Drivers -Wall
LDFLAGS = -L./Drivers -lGenericDrivers -lpigpio -lrt

# Target binary
TARGET = gpio_control

# Source files
SRC = gpio_control.cpp
OBJ = $(SRC:.cpp=.o)

# Rule for making the actual target
$(TARGET): $(OBJ)
	$(CXX) $(OBJ) -o $(TARGET) $(LDFLAGS)

# Rule for object files
%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Phony targets for clean and making the library
.PHONY: clean drivers

# Rule to make the drivers library
drivers:
	$(MAKE) -C Drivers

# Clean rule
clean:
	$(RM) $(OBJ) $(TARGET)
	$(MAKE) -C Drivers clean

# Include dependency files
-include $(OBJ:.o=.d)

# Rule to generate a dependency file by using the C preprocessor
%.d: %.cpp
	@$(CPP) $(CXXFLAGS) $< -MM -MT $(@:.d=.o) >$@

# Make the drivers library as well
all: drivers $(TARGET)