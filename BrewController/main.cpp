#include <iostream>
#include <stdio.h>
#include "Rpc.h"
#include "BrewController.h"
#include "BrewFSM.h"
#include "SystemFSM.h"


int main(int argc, char *argv[])
{
    int ret = 0;
    Rpc rpc(9050);
    rpc.start();
    SystemFSM systemFsm;
    systemFsm.run();


    return 0;
}