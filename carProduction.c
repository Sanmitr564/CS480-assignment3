#include <string.h>
#include <stdio.h>

#include "carProduction.h"
#include "stdlib.h"

PowertrainConveyor* newPowertrainConveyor(){
    Queue* powertrainQueue = newQueue();
    

    sem_t* powertrainMutex = (sem_t*)malloc(sizeof(sem_t));
    sem_t* powertrainEmpty = (sem_t*)malloc(sizeof(sem_t));
    sem_t* powertrainFull = (sem_t*)malloc(sizeof(sem_t));
    sem_t* hybridEmpty = (sem_t*)malloc(sizeof(sem_t));

    if(
        sem_init(powertrainMutex, 0, 1) == -1 ||
        sem_init(powertrainEmpty, 0, PowertrainConveyorMax) == -1 ||
        sem_init(powertrainFull, 0, 0) == -1 ||
        sem_init(hybridEmpty, 0, HybridMax) == -1
    ){
        printf("Couldn't initialize semaphores for PowertrainConveyor");
        exit(-1);
    }

    unsigned int* produced = (unsigned int*)malloc(PowertrainTypeN * sizeof(int));
    unsigned int* consumed = (unsigned int*)malloc(PowertrainTypeN * sizeof(int));
    for(int i = 0; i < PowertrainTypeN; i++){
        produced[i] = 0;
        consumed[i] = 0;
    }

    PowertrainConveyor* newConveyor = (PowertrainConveyor*)(malloc(sizeof(PowertrainConveyor)));
    
    newConveyor->powertrainQueue = powertrainQueue;
    newConveyor->powertrainMutex = powertrainMutex;
    newConveyor->powertrainEmpty = powertrainEmpty;
    newConveyor->powertrainFull = powertrainFull;
    newConveyor->hybridEmpty = hybridEmpty;
    newConveyor->produced = produced;
    newConveyor->consumed = consumed;

    return newConveyor;
}

PoweredChassisConveyor* newPoweredChassisConveyor(){
    Queue* queue = newQueue();

    sem_t* chassisMutex = (sem_t*)malloc(sizeof(sem_t));
    sem_t* chassisEmpty = (sem_t*)malloc(sizeof(sem_t));
    sem_t* chassisFull = (sem_t*)malloc(sizeof(sem_t));
    if(
        sem_init(chassisMutex, 0, 1) == -1 ||
        sem_init(chassisEmpty, 0, PoweredChassisConveyorMax) == -1 ||
        sem_init(chassisFull, 0, 0) == -1
    ){
        printf("Couldn't initialize semaphores for PoweredChassisConveyor");
        exit(-1);
    }

    unsigned int** consumed = (unsigned int**)malloc(ChassisRobotTypeN * sizeof(int*));
    for(int i = 0; i < ChassisRobotTypeN; i++){
        consumed[i] = (unsigned int*)malloc(PowertrainTypeN * sizeof(int));
        for(int j = 0; j < PowertrainTypeN; j++){
            consumed[i][j] = 0;
        }
    }

    PoweredChassisConveyor* newConveyor = (PoweredChassisConveyor*)malloc(sizeof(PoweredChassisConveyor));
    newConveyor->poweredChassisQueue = queue;
    newConveyor->chassisMutex = chassisMutex;
    newConveyor->chassisEmpty = chassisEmpty;
    newConveyor->chassisFull = chassisFull;
    newConveyor->produced = 0;
    newConveyor->consumed = consumed;
    return newConveyor;
}

PowertrainProducer* newPowertrainProducer(PowertrainConveyor* conveyor, int numToProduce, unsigned int sleep){
    PowertrainProducer* newProducer = (PowertrainProducer*)(malloc(sizeof(PowertrainProducer)));
    newProducer->powertrainConveyor = conveyor;
    newProducer->numToProduce = numToProduce;
    newProducer->sleep = sleep;

    return newProducer;
}

PoweredChassisProducer* newPoweredChassisProducer(PowertrainConveyor* powerTrainConveyor, PoweredChassisConveyor* poweredChassisConveyer, int numToProduce, unsigned int sleep){
    PoweredChassisProducer* conveyor = (PoweredChassisProducer*)malloc(sizeof(PoweredChassisProducer));
    conveyor->powertrainConveyor = powerTrainConveyor;
    conveyor->poweredChassisConveyor = poweredChassisConveyer;
    conveyor->numToProduce = numToProduce;
    conveyor->sleep = sleep;

    return conveyor;
}

PoweredChassisConsumer* newPoweredChassisConsumer(PoweredChassisConveyor* conveyor, int numToProduce, unsigned int sleep){
    sem_t* barrier = (sem_t*)malloc(sizeof(sem_t));
    if(sem_init(barrier, 0, 0) == -1){
        printf("Couldn't initialize semaphore for PoweredChassisConsumer");
        exit(-1);
    }

    PoweredChassisConsumer* consumer = (PoweredChassisConsumer*)malloc(sizeof(PoweredChassisConsumer));
    consumer->poweredChassisConveyor = conveyor;
    consumer->barrier = barrier;
    consumer->numToProduce = numToProduce;
    consumer->sleep = sleep;

    return consumer;
}
