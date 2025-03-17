#include <stdio.h>
#include <unistd.h>
#include <pthread.h>
#include <semaphore.h>
#include <stdlib.h>

#include <string.h>
#include <pthread.h>
// #include <fstream>
// #include <iostream>

#include "car_assemble.h"
#include "log.h"
#include "carProduction.h"


int main(int argc, char **argv){
    int numVehicles = 100;
    int gasProduceTime = 0;
    int hybridProduceTime = 0;
    int titanoTime = 0;
    int megaTime = 0;
    int roboTime = 0;
    int option;

    while((option = getopt(argc, argv, "n:g:h:t:m:r:")) != -1){
        switch(option){
            case 'n':
                numVehicles = atoi(optarg);
                break;
            
            case 'g':
                gasProduceTime = atoi(optarg);
                break;
            
            case 'h':
                hybridProduceTime = atoi(optarg);
                break;
            
            case 't':
                titanoTime = atoi(optarg);
                break;
            
            case 'm':
                megaTime = atoi(optarg);
                break;
            
            case 'r':
                roboTime = atoi(optarg);
                break;
            default:
                return -1;
        }
    }

     
    PowertrainConveyor* powertrainConveyor = newPowertrainConveyor();
    PoweredChassisConveyor* poweredChassisConveyor = newPoweredChassisConveyor();
    PowertrainProducer* gasInfo = newPowertrainProducer(powertrainConveyor, numVehicles, gasProduceTime);
    PowertrainProducer* hybridInfo = newPowertrainProducer(powertrainConveyor, numVehicles, hybridProduceTime);
    PoweredChassisProducer* titanoInfo = newPoweredChassisProducer(powertrainConveyor, poweredChassisConveyor, numVehicles, titanoTime);
    PoweredChassisProducer* megaForceInfo = newPoweredChassisProducer(powertrainConveyor, poweredChassisConveyor, numVehicles, megaTime);
    PoweredChassisConsumer* roboMountInfo = newPoweredChassisConsumer(poweredChassisConveyor, numVehicles, roboTime);

    pthread_t gasThread, hybridThread, titanoThread, megaThread, roboThread;

    pthread_create(&gasThread, NULL, gasEngine, gasInfo);
    pthread_create(&hybridThread, NULL, hybridEngine, hybridInfo);
    pthread_create(&titanoThread, NULL, titanoRobot, titanoInfo);
    pthread_create(&megaThread, NULL, megaForceRobot, megaForceInfo);
    pthread_create(&roboThread, NULL, roboMountRobot, roboMountInfo);

    sem_wait(roboMountInfo->barrier);

    log_powertrain_history(powertrainConveyor->produced, poweredChassisConveyor->consumed);

    return 0;
}

