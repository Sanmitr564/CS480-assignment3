#include <string.h>
#include <stdio.h>

#include "carProduction.h"
#include "stdlib.h"

/**
 * @brief initializes new PowertrainConveyor and fields to be shared
 * 
 * @return initialized PowertrainConveyor* 
 */
PowertrainConveyor* newPowertrainConveyor(){
    //create new queue
    Queue* powertrainQueue = newQueue();
    
    //create semaphores
    sem_t* powertrainMutex = (sem_t*)malloc(sizeof(sem_t));
    sem_t* powertrainEmpty = (sem_t*)malloc(sizeof(sem_t));
    sem_t* powertrainFull = (sem_t*)malloc(sizeof(sem_t));
    sem_t* hybridEmpty = (sem_t*)malloc(sizeof(sem_t));

    //initialize semaphores
    if(
        sem_init(powertrainMutex, 0, 1) == -1 ||
        sem_init(powertrainEmpty, 0, PowertrainConveyorMax) == -1 ||
        sem_init(powertrainFull, 0, 0) == -1 ||
        sem_init(hybridEmpty, 0, HybridMax) == -1
    ){
        printf("Couldn't initialize semaphores for PowertrainConveyor");
        exit(-1);
    }

    //create and initialize arrays
    unsigned int* produced = (unsigned int*)malloc(PowertrainTypeN * sizeof(int));
    unsigned int* consumed = (unsigned int*)malloc(PowertrainTypeN * sizeof(int));
    for(int i = 0; i < PowertrainTypeN; i++){
        produced[i] = 0;
        consumed[i] = 0;
    }

    //create PowertrainConveyor
    PowertrainConveyor* newConveyor = (PowertrainConveyor*)(malloc(sizeof(PowertrainConveyor)));
    
    //populate fields
    newConveyor->powertrainQueue = powertrainQueue;
    newConveyor->powertrainMutex = powertrainMutex;
    newConveyor->powertrainEmpty = powertrainEmpty;
    newConveyor->powertrainFull = powertrainFull;
    newConveyor->hybridEmpty = hybridEmpty;
    newConveyor->produced = produced;
    newConveyor->consumed = consumed;

    //return conveyor
    return newConveyor;
}

/**
 * @brief initializes new PoweredChassisConveyor and fields to be shared
 * 
 * @return initialized PoweredChassisConveyor* 
 */
PoweredChassisConveyor* newPoweredChassisConveyor(){
    //create new queue
    Queue* queue = newQueue();

    //create semaphores
    sem_t* chassisMutex = (sem_t*)malloc(sizeof(sem_t));
    sem_t* chassisEmpty = (sem_t*)malloc(sizeof(sem_t));
    sem_t* chassisFull = (sem_t*)malloc(sizeof(sem_t));

    //initialize semaphores
    if(
        sem_init(chassisMutex, 0, 1) == -1 ||
        sem_init(chassisEmpty, 0, PoweredChassisConveyorMax) == -1 ||
        sem_init(chassisFull, 0, 0) == -1
    ){
        printf("Couldn't initialize semaphores for PoweredChassisConveyor");
        exit(-1);
    }

    //create and initialize array
    unsigned int** consumed = (unsigned int**)malloc(ChassisRobotTypeN * sizeof(int*));
    for(int i = 0; i < ChassisRobotTypeN; i++){
        consumed[i] = (unsigned int*)malloc(PowertrainTypeN * sizeof(int));
        for(int j = 0; j < PowertrainTypeN; j++){
            consumed[i][j] = 0;
        }
    }

    //create PoweredChassisConveyor
    PoweredChassisConveyor* newConveyor = (PoweredChassisConveyor*)malloc(sizeof(PoweredChassisConveyor));

    //populate fields
    newConveyor->poweredChassisQueue = queue;
    newConveyor->chassisMutex = chassisMutex;
    newConveyor->chassisEmpty = chassisEmpty;
    newConveyor->chassisFull = chassisFull;
    newConveyor->produced = 0;
    newConveyor->consumed = consumed;

    //return conveyor
    return newConveyor;
}

/**
 * @brief initializes new robot specific PowertrainProducer struct and fields
 * 
 * @param conveyor shared PowertrainConveyor robot has access to
 * @param numToProduce how many total powertrains the robots need to produce
 * @param sleep how long to sleep in milliseconds
 * 
 * @return initialized PowertrainProducer* 
 */
PowertrainProducer* newPowertrainProducer(PowertrainConveyor* conveyor, int numToProduce, unsigned int sleep){
    //create PowertrainProducer
    PowertrainProducer* newProducer = (PowertrainProducer*)(malloc(sizeof(PowertrainProducer)));

    //populate fields
    newProducer->powertrainConveyor = conveyor;
    newProducer->numToProduce = numToProduce;
    newProducer->sleep = sleep;

    //return producer
    return newProducer;
}

/**
 * @brief initializes new robot specific PoweredChassisProducer struct and fields
 * 
 * @param powerTrainConveyor shared PowertrainConveyor robot has access to
 * @param poweredChassisConveyer shared PoweredChassisConveyor robot has access to
 * @param numToProduce how many total powered chassis the robots need to produce
 * @param sleep how long to sleep in milliseconds
 * 
 * @return initialized PoweredChassisProducer* 
 */
PoweredChassisProducer* newPoweredChassisProducer(PowertrainConveyor* powerTrainConveyor, PoweredChassisConveyor* poweredChassisConveyer, int numToProduce, unsigned int sleep){
    //create PoweredChassisProducer
    PoweredChassisProducer* newProducer = (PoweredChassisProducer*)malloc(sizeof(PoweredChassisProducer));

    //populate fields
    newProducer->powertrainConveyor = powerTrainConveyor;
    newProducer->poweredChassisConveyor = poweredChassisConveyer;
    newProducer->numToProduce = numToProduce;
    newProducer->sleep = sleep;

    //return producer
    return newProducer;
}

/**
 * @brief initializes new robot specific PoweredChassisConsumer struct and fields
 * 
 * @param conveyor shared PoweredChassisConveyor robot has access to
 * @param numToProduce how many total powered chassis the robots need to produce
 * @param sleep how long to sleep in milliseconds
 * @return initialized PoweredChassisConsumer* 
 */
PoweredChassisConsumer* newPoweredChassisConsumer(PoweredChassisConveyor* conveyor, int numToProduce, unsigned int sleep){

    //create barrier semaphore
    sem_t* barrier = (sem_t*)malloc(sizeof(sem_t));

    //initialize barrier semaphore
    if(sem_init(barrier, 0, 0) == -1){
        printf("Couldn't initialize semaphore for PoweredChassisConsumer");
        exit(-1);
    }

    //create PoweredChassisConsumer
    PoweredChassisConsumer* consumer = (PoweredChassisConsumer*)malloc(sizeof(PoweredChassisConsumer));

    //populate fields
    consumer->poweredChassisConveyor = conveyor;
    consumer->barrier = barrier;
    consumer->numToProduce = numToProduce;
    consumer->sleep = sleep;

    //return consumer
    return consumer;
}
