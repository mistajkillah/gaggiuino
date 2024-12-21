#include "GenericDrivers.h"
#include "Spi.h"
#include "I2cBus.h"
#include "I2cBusLinux.h"
#include "ADS1X15.h"

#include "MAX6675_TempSensor.h"

#include <pigpio.h>
#include <iostream>
#include <sqlite3.h>
// #define _GNU_SOURCE
#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdbool.h>
#include <math.h>
#include <sched.h>
#include <sys/types.h>
#include <iostream>
#include <chrono>
#include <thread>
int SIM = true;

#define SIMULATION \
    if (SIM)       \
    {              \
        return 0;  \
    }

typedef std::chrono::high_resolution_clock HiresClock;
typedef std::chrono::system_clock Clock;
const int TOTAL_ITERATIONS = 6000;
const double NANO_TO_MSEC = 1000000.0;

const int inputPin = 22;  // PIN 15 interupt pin
const int outputPin = 27; // PIN 13 response to interupt pin
const int togglePin = 17; // PIN 11 fake clock/interupt agressor

// Callback function to be called when the input pin changes state
void inputCallback(int gpio, int level, uint32_t tick)
{
    // Set the output pin based on the input pin level
    gpioWrite(outputPin, level);
}
void ReadADC()
{

    // I2cBusLinux i2c(0,"i2c0");
    // ADS1015 adc(&i2c,"WaterTemp",0x48);
    // adc.begin();
    // adc.setGain(0);
    //   int16_t val_0 = adc.readADC(0);
    //   int16_t val_1 = adc.readADC(1);
    //   int16_t val_2 = adc.readADC(2);
    //   int16_t val_3 = adc.readADC(3);

    //   float f = adc.toVoltage(1);  // voltage factor

    //   printf("Analog0: 0x%X  %f\n",val_0,val_0 * f);
    //   printf("Analog1: 0x%X  %f\n",val_1,val_1 * f);
    //   printf("Analog2: 0x%X  %f\n",val_2,val_2* f);
    //   printf("Analog3: 0x%X  %f\n",val_3,val_3 * f);
}

void ReadTemp()
{

    //   SpiDeviceLinux  spiDevice(0,0,4000000,8,1000,0,"SpiDevice0.0");
    //   MAX6675_TempSensor tempSensor(&spiDevice,"BoilerTemp");
    //   printf("Temperature %lf\n",tempSensor.readCelsius());
}

int Init()
{
    SIMULATION
    // Initialize pigpio library
    if (gpioInitialise() < 0)
    {
        std::cerr << "Failed to initialize pigpio" << std::endl;
        return 1;
    }

    // Set GPIO modes
    gpioSetMode(inputPin, PI_INPUT);
    gpioSetMode(outputPin, PI_OUTPUT);
    gpioSetMode(togglePin, PI_OUTPUT);

    // Set pull-down resistor on the input pin
    gpioSetPullUpDown(inputPin, PI_PUD_DOWN);

    // Set the callback function for the input pin
    gpioSetAlertFunc(inputPin, inputCallback);
    return 0;
}
// calculate latency between each pair of iterations --> EXPECTED - ACTUAL = EXPECTED - (T_n - T_n-1)
double calculate_latency(std::chrono::time_point<HiresClock> curr_struct_start, std::chrono::time_point<HiresClock> prev_struct_start)
{
    return (std::chrono::duration_cast<std::chrono::nanoseconds>(curr_struct_start.time_since_epoch()).count() / NANO_TO_MSEC - std::chrono::duration_cast<std::chrono::nanoseconds>(prev_struct_start.time_since_epoch()).count() / NANO_TO_MSEC) - 10;
}

// type of data for csv file
typedef struct
{
    int index;

    double sleeptime;
    int math_sum;
    bool exceeds_ten;
    std::chrono::time_point<HiresClock> start;
    std::chrono::time_point<HiresClock> end;
    std::chrono::duration<double, std::nano> elapsed;
    ;
    double latency;

} Iteration;

// our math operations: read from urandom and find sum,
// then add to struct's math_sum property
static inline void operation(Iteration *struct_ptr)
{
    int random_nums = open("/dev/urandom", O_RDONLY);
    if (random_nums < 0)
    {
        perror("Error opening urandom file!\n");
    }
    else
    {
        int array_len = 100;
        int my_rand_data[array_len];
        ssize_t data_result = read(random_nums, my_rand_data, sizeof my_rand_data);
        close(random_nums);
    }
}
static inline void execute_sleep(Iteration *struct_ptr, int index)
{
    long max_ns_sleep = 10000000; // 10ms
    struct timespec tim1, tim2;
    double sleep_time = 0.0;
    if (struct_ptr[index].elapsed.count() > max_ns_sleep)
    {
        struct_ptr[index].exceeds_ten = true;
        perror("exceeded");
    }
    else
    {
        sleep_time = max_ns_sleep - (struct_ptr[index].elapsed.count());
        struct_ptr[index].sleeptime = sleep_time;
        std::this_thread::sleep_for(std::chrono::nanoseconds(static_cast<long long>(sleep_time)));
    }
}


void ControlLoop()
{
    Iteration all_structs[TOTAL_ITERATIONS];
    int curr_index = 0;
    auto t1 = Clock::now();
    auto t2 = Clock::now();
    auto t1_tick = HiresClock::now();
    auto t2_tick = HiresClock::now();

    while (curr_index < TOTAL_ITERATIONS)
    {

        printf("In Loop index is: %d\n", curr_index);
        all_structs[curr_index].index = curr_index;
        printf("all_structs index is: %d\n", all_structs[curr_index].index);
        memset(&(all_structs[curr_index]), 0, sizeof(Iteration));
        all_structs[curr_index].start = HiresClock::now();

        operation(&all_structs[curr_index]);

        if (curr_index != 0)
        {
            all_structs[curr_index].latency = calculate_latency(all_structs[curr_index].start, all_structs[curr_index - 1].start);
        }
        else
        {
            all_structs[curr_index].latency = 0;
        }

        all_structs[curr_index].end = HiresClock::now();
        all_structs[curr_index].index = curr_index;
        all_structs[curr_index].elapsed = all_structs[curr_index].end - all_structs[curr_index].start;
        execute_sleep(all_structs, curr_index);
        curr_index++;
    }
}
int main(int argc, char *argv[])
{
    int ret = 0;
    ret = Init();
    ControlLoop();
    return 0;
}
//  }
// int main(int argc, char*agv[])
// {
//     // Set up for toggling the third pin at 120 Hz
//     const int toggleFrequency = 120;  // Toggle frequency in Hz
//     const uint32_t toggleInterval = 1000000 / toggleFrequency;  // Interval in microseconds
//     uint32_t lastToggleTime = gpioTick();  // Last toggle time

//     std::cout << "Starting loop, press CTRL+C to exit..." << std::endl;

//     // Main loop
//     while (1) {
//         uint32_t currentTime = gpioTick();
//         // Check if it's time to toggle the pin
//         if (currentTime - lastToggleTime >= toggleInterval) {
//             // Read the current state and write the inverse
//             gpioWrite(togglePin, !gpioRead(togglePin));
//             lastToggleTime = currentTime;
//         }

//         // Small delay to prevent 100% CPU utilization in the loop
//         gpioDelay(100);  // Delay for 1 millisecond
//     }

//     // Cleanup the pigpio resources
//     gpioTerminate();
//     return 0;
// }
