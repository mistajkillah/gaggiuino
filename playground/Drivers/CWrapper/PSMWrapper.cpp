#include <stdint.h>
#include "PSMWrapper.h"
#include "../PSM.h" // Include the original C++ header

#ifdef __cplusplus
extern "C" {
#endif

// Function to create a PSM instance
PSMHandle PSM_Create(unsigned char sensePin, unsigned char controlPin, unsigned int range, int mode, unsigned char divider, unsigned char interruptMinTimeDiff) {
    // Allocate memory for the PSM instance
    PSM* psm = new PSM(sensePin, controlPin, range, mode, divider, interruptMinTimeDiff);

    // Return a pointer to the PSM instance as a handle
    return (PSMHandle)psm;
}

// Function to set the PSM value
void PSM_Set(PSMHandle handle, unsigned int value) {
    PSM* psm = (PSM*)handle;
    if (psm) {
        psm->set(value);
    }
}

// Function to get the PSM counter
long PSM_GetCounter(PSMHandle handle) {
    PSM* psm = (PSM*)handle;
    if (psm) {
        return psm->getCounter();
    }
    return 0;
}

// Function to reset the PSM counter
void PSM_ResetCounter(PSMHandle handle) {
    PSM* psm = (PSM*)handle;
    if (psm) {
        psm->resetCounter();
    }
}

// Function to stop the PSM after a specific counter value
void PSM_StopAfter(PSMHandle handle, long counter) {
    PSM* psm = (PSM*)handle;
    if (psm) {
        psm->stopAfter(counter);
    }
}

// Function to calculate CPS (Counts Per Second)
unsigned int PSM_CPS(PSMHandle handle) {
    PSM* psm = (PSM*)handle;
    if (psm) {
        return psm->cps();
    }
    return 0;
}

// Function to get the last millis
unsigned long PSM_GetLastMillis(PSMHandle handle) {
    PSM* psm = (PSM*)handle;
    if (psm) {
        return psm->getLastMillis();
    }
    return 0;
}

// Function to get the PSM divider
unsigned char PSM_GetDivider(PSMHandle handle) {
    PSM* psm = (PSM*)handle;
    if (psm) {
        return psm->getDivider();
    }
    return 0;
}

// Function to set the PSM divider
void PSM_SetDivider(PSMHandle handle, unsigned char divider) {
    PSM* psm = (PSM*)handle;
    if (psm) {
        psm->setDivider(divider);
    }
}

// Function to shift the PSM divider counter
void PSM_ShiftDividerCounter(PSMHandle handle, char value) {
    PSM* psm = (PSM*)handle;
    if (psm) {
        psm->shiftDividerCounter(value);
    }
}

// Function to destroy the PSM instance
void PSM_Destroy(PSMHandle handle) {
    PSM* psm = (PSM*)handle;
    if (psm) {
        delete psm;
    }
}


#ifdef __cplusplus
}
#endif
