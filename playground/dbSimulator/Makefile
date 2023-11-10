CXX = g++
CXXFLAGS = -std=c++11 -Wall -Wextra
LDFLAGS = -lsqlite3 -lpthread

SRC = BrewDBClass.cpp
HEADERS = BrewDBClass.h
TARGET = brew_program

all: $(TARGET)

$(TARGET): $(SRC) $(HEADERS)
	$(CXX) $(CXXFLAGS) $(SRC) -o $(TARGET) $(LDFLAGS)

clean:
	rm -f $(TARGET)

.PHONY: all clean
