#include <iostream>
#include <fstream>
#include <stdio.h>
#include "Rpc.h"

#include "BrewController.h"
#include "BrewHW.h"
#include "SystemFSM.h"

using namespace std;


int main(int argc, char *argv[])
{
   // int ret = 0;
    //Rpc rpc(9050);
    //rpc.start();
    
    (void)argc;
    (void)argv;
    SystemFSM systemFsm;
    systemFsm.run();


    return 0;
}