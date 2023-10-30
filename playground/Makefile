# Define the name of your C++ program
TARGET = gpio_control

# Specify the source and object files
SRCS = gpio_control.cpp
OBJS = $(SRCS:.cpp=.o)

# Compiler and linker options
CXX = g++
CXXFLAGS = -Wall -Wextra -std=c++11
LDFLAGS = -lpigpio -lrt

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CXX) $(CXXFLAGS) -o $@ $^ $(LDFLAGS)

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c -o $@ $<

clean:
	rm -f $(OBJS) $(TARGET)

.PHONY: all clean
