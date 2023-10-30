# Define the compiler and flags
CC = g++
CXXFLAGS = -IInc -I. -ICommon -Wall -std=c++11 

# Define the source files and object files
SRC = gaggiuino.cpp log.cpp eeprom_data/eeprom_data.cpp functional/descale.cpp\
functional/just_do_coffee.cpp\
peripherals/pressure_sensor.cpp peripherals/pump.cpp\
peripherals/remote_scales.cpp  PSM/PSM.cpp SimpleKalmanFilter/SimpleKalmanFilter.cpp\
Common/measurements.cpp Common/profiling_phases.cpp Common/utils.cpp MAX6675/max6675.cpp movingAvg/movingAvg.cpp\
ADS1X15/ADS1X15.cpp

OBJ = $(SRC:.cpp=.o)

# Define the executable name
EXE = gaggiuino

# Define the rule to make the executable
$(EXE): $(OBJ)
	$(CC) $(CXXFLAGS) -o $@ $^

# Define the rule to make the object files
%.o: %.cpp
	$(CC) $(CXXFLAGS) -c $< -o $@

# Define the rule to clean the object files and executable
clean:
	rm -f $(OBJ) $(EXE)