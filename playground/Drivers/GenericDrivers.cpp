#include <unistd.h>

void delay(long msec)
{
  usleep(msec *1000);
}