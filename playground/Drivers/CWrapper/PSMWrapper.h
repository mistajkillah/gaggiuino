#pragma once

#ifdef __cplusplus
extern "C" {
#endif

// Define a C-compatible handle for PSM
typedef void* PSMHandle;

// Function to create a PSM instance
PSMHandle PSM_Create(unsigned char sensePin, unsigned char controlPin, unsigned int range, int mode, unsigned char divider, unsigned char interruptMinTimeDiff);

// Function to set the PSM value
void PSM_Set(PSMHandle handle, unsigned int value);

// Function to get the PSM counter
long PSM_GetCounter(PSMHandle handle);

// Function to reset the PSM counter
void PSM_ResetCounter(PSMHandle handle);

// Function to stop the PSM after a specific counter value
void PSM_StopAfter(PSMHandle handle, long counter);

// Function to calculate CPS (Counts Per Second)
unsigned int PSM_CPS(PSMHandle handle);

// Function to get the last millis
unsigned long PSM_GetLastMillis(PSMHandle handle);

// Function to get the PSM divider
unsigned char PSM_GetDivider(PSMHandle handle);

// Function to set the PSM divider
void PSM_SetDivider(PSMHandle handle, unsigned char divider);

// Function to shift the PSM divider counter
void PSM_ShiftDividerCounter(PSMHandle handle, char value);

// Function to destroy the PSM instance
void PSM_Destroy(PSMHandle handle);

#ifdef __cplusplus
}
#endif


