#!/bin/bash
make clean; make ; make generate_bindings; cp src/generated/* ../../BrewController/src/; cp libGenericDrivers.so ../../BrewController/libGenericDrivers.so ; sudo cp libGenericDrivers.so /lib/

