#pragma once
int SIM = true;

#define SIMULATION \
    if (SIM)       \
    {              \
        return 0;  \
    }