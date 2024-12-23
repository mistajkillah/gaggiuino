#include <iostream>
#include <stdio.h>
#include "Rpc.h"
#include "BrewController.h"
#include "BrewHW.h"
#include "BrewFSM.h"
#include "SystemFSM.h"

using namespace std;
int main(int argc, char *argv[])
{
   // int ret = 0;
    Rpc rpc(9050);
    rpc.start();
    
    BrewHW &hw=BrewHW::getInstance();
    hw.initializeHW();
    SensorState state=hw.getSensorState();
    cout<<state.toString()<<endl;
    //SystemFSM systemFsm;
    //systemFsm.run();


    return 0;
}