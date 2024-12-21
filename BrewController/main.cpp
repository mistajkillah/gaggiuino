#include <iostream>
#include <stdio.h>
#include "BrewController.h"
#include "BrewFSM.h"
#include "SystemFSM.h"

int main(int argc, char *argv[])
{
    int ret = 0;

    SystemFSM systemFsm;
    systemFsm.run();

    return 0;
}