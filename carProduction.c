#include "carProduction.h"
#include "stdlib.h"
#include "car_assemble.h"
#include "queue.h"
#include <stdbool.h>
#include <time.h>
#include "log.h"
#include <string.h>

PowertrainConveyor* newPowertrainConveyor(){
    Queue* powertrainQueue = newQueue();
    

    sem_t* powertrainMutex = (sem_t*)malloc(sizeof(sem_t));
    sem_t* powertrainEmpty = (sem_t*)malloc(sizeof(sem_t));
    sem_t* powertrainFull = (sem_t*)malloc(sizeof(sem_t));
    sem_t* hybridEmpty = (sem_t*)malloc(sizeof(sem_t));
    sem_t* hybridFull = (sem_t*)malloc(sizeof(sem_t));
    if(
        sem_init(powertrainMutex, 0, 1) == -1 ||
        sem_init(powertrainEmpty, 0, PowertrainConveyorMax) == -1 ||
        sem_init(powertrainFull, 0, 0) == -1 ||
        sem_init(hybridEmpty, 0, HybridMax) == -1 ||
        sem_init(hybridFull, 0, 0) == -1
    ){
        printf("Couldn't initialize semaphores for PowertrainConveyor");
        exit(-1);
    }

    unsigned int* produced = (int*)malloc(PowertrainTypeN * sizeof(int));
    unsigned int* consumed = (int*)malloc(PowertrainTypeN * sizeof(int));
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
    newConveyor->hybridFull = hybridFull;
    newConveyor->produced = produced;
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

PoweredChassisConsumer* newPoweredChassisConveyor(){
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

    unsigned int** consumed = (int**)malloc(ChassisRobotTypeN * sizeof(int*));
    for(int i = 0; i < ChassisRobotTypeN; i++){
        consumed[i] = (int*)malloc(PowertrainTypeN * sizeof(int));
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

PoweredChassisProducer* newPoweredChassisProducer(PowertrainConveyor* powerTrainConveyor, PoweredChassisConveyor* poweredChassisConveyer, int numToProduce, unsigned int sleep){
    PoweredChassisProducer* conveyor = (PoweredChassisProducer*)malloc(sizeof(PoweredChassisProducer));
    conveyor->powertrainConveyor = powerTrainConveyor;
    conveyor->poweredChassisConveyor = poweredChassisConveyer;
    conveyor->numToProduce = numToProduce;
    conveyor->sleep = sleep;

    return conveyor;
}

void* gasEngine(void *ptr){
    PowertrainProducer* info = (PowertrainProducer*)ptr;
    struct timespec sleepTime = {0, info->sleep * 1000000};
    while(true){
        nanosleep(sleepTime, NULL);

        sem_wait(info->powertrainConveyor->powertrainEmpty);
        sem_wait(info->powertrainConveyor->powertrainMutex);

        int produced = 0;
        for(int i = 0; i < PowertrainTypeN; i++){
            produced += info->powertrainConveyor->produced[i];
        }
        if(info->numToProduce == produced){
            break; 
        }

        Node* node = newNode(powertrain_producerNames[GasEngine], GasEngine, NULL);
        enqueue(info->powertrainConveyor->powertrainQueue, node);
        info->powertrainConveyor->produced[GasEngine]++;
        
        int* inAssemblyQueue = (int*)malloc(PowertrainTypeN * sizeof(int));
        for(int i = 0; i < PowertrainTypeN; i++){
            inAssemblyQueue[i] = info->powertrainConveyor->produced[i] - info->powertrainConveyor->consumed[i];
        }
        PowertrainAdded powertrain = {GasEngine, info->powertrainConveyor->produced, inAssemblyQueue};
        log_added_powertrain(powertrain);
        free(inAssemblyQueue);

        sem_post(info->powertrainConveyor->powertrainMutex);
        sem_post(info->powertrainConveyor->powertrainFull);
    }
}

void* hybridEngine(void *ptr){
    PowertrainProducer* info = (PowertrainProducer*)ptr;
    struct timespec sleepTime = {0, info->sleep * 1000000};
    while(true){
        nanosleep(sleepTime, NULL);

        sem_wait(info->powertrainConveyor->hybridEmpty);
        sem_wait(info->powertrainConveyor->powertrainEmpty);
        sem_wait(info->powertrainConveyor->powertrainMutex);

        int produced = 0;
        for(int i = 0; i < PowertrainTypeN; i++){
            produced += info->powertrainConveyor->produced[i];
        }
        if(info->numToProduce == produced){
            break; 
        }

        Node* node = newNode(powertrain_producerNames[HybridEngine], HybridEngine, NULL);
        enqueue(info->powertrainConveyor->powertrainQueue, node);
        info->powertrainConveyor->produced[HybridEngine]++;
        
        int* inAssemblyQueue = (int*)malloc(PowertrainTypeN * sizeof(int));
        for(int i = 0; i < PowertrainTypeN; i++){
            inAssemblyQueue[i] = info->powertrainConveyor->produced[i] - info->powertrainConveyor->consumed[i];
        }
        PowertrainAdded powertrain = {HybridEngine, info->powertrainConveyor->produced, inAssemblyQueue};
        log_added_powertrain(powertrain);
        free(inAssemblyQueue);

        sem_post(info->powertrainConveyor->powertrainMutex);
        sem_post(info->powertrainConveyor->powertrainFull);
        sem_post(info->powertrainConveyor->hybridFull);
    }
}

void* titanoRobot(void* ptr){
    PoweredChassisProducer* info = (PoweredChassisProducer*)ptr;
    struct timespec sleepTime = {0, info->sleep * 1000000};
    while(true){
        sem_wait(info->powertrainConveyor->powertrainFull);
        sem_wait(info->powertrainConveyor->powertrainMutex);

        int consumed = 0;
        for(int i = 0; i < PowertrainTypeN; i++){
            consumed += info->powertrainConveyor->consumed[i];
        }
        if(info->numToProduce == consumed){
            break;
        }

        Node* node = dequeue(info->powertrainConveyor->powertrainQueue);
        info->poweredChassisConveyor->consumed[node->trainType]++;

        int* inAssemblyQueue = (int*)malloc(PowertrainTypeN * sizeof(int));
        for(int i = 0; i < PowertrainTypeN; i++){
            inAssemblyQueue[i] = info->powertrainConveyor->produced[i] - info->powertrainConveyor->consumed[i];
        }
        PowertrainRemoved powertrain = {Titano, node->trainType, info->powertrainConveyor->consumed, inAssemblyQueue};
        log_removed_powertrain(powertrain);
        free(inAssemblyQueue);

        sem_post(info->powertrainConveyor->powertrainMutex);
        sem_post(info->powertrainConveyor->powertrainEmpty);
        if(node->trainType == HybridEngine){
            sem_post(info->powertrainConveyor->hybridEmpty);
        }

        nanosleep(sleepTime, NULL);

        sem_wait(info->poweredChassisConveyor->chassisEmpty);
        sem_wait(info->poweredChassisConveyor->chassisMutex);

        char str[50];
        strcpy(str, powertrain_consumerNames[Titano]);
        strcat(str, powertrain_producerNames[node->trainType]);

        Node* chassisNode = newNode(str, node->trainType, Titano);
        enqueue(info->poweredChassisConveyor->poweredChassisQueue, chassisNode);
        info->poweredChassisConveyor->produced++;
        
        log_added_poweredchassis(chassisNode->string, info->poweredChassisConveyor->poweredChassisQueue->length);
        free(node);
        sem_post(info->poweredChassisConveyor->chassisMutex);
        sem_post(info->poweredChassisConveyor->chassisEmpty);
    }
}

void* megaForceRobot(void *ptr){
    PoweredChassisProducer* info = (PoweredChassisProducer*)ptr;
    struct timespec sleepTime = {0, info->sleep * 1000000};
    while(true){
        sem_wait(info->powertrainConveyor->powertrainFull);
        sem_wait(info->powertrainConveyor->powertrainMutex);

        int consumed = 0;
        for(int i = 0; i < PowertrainTypeN; i++){
            consumed += info->powertrainConveyor->consumed[i];
        }
        if(info->numToProduce == consumed){
            break;
        }

        Node* node = dequeue(info->powertrainConveyor->powertrainQueue);
        info->poweredChassisConveyor->consumed[node->trainType]++;

        int* inAssemblyQueue = (int*)malloc(PowertrainTypeN * sizeof(int));
        for(int i = 0; i < PowertrainTypeN; i++){
            inAssemblyQueue[i] = info->powertrainConveyor->produced[i] - info->powertrainConveyor->consumed[i];
        }
        PowertrainRemoved powertrain = {MegaForce, node->trainType, info->powertrainConveyor->consumed, inAssemblyQueue};
        log_removed_powertrain(powertrain);
        free(inAssemblyQueue);

        sem_post(info->powertrainConveyor->powertrainMutex);
        sem_post(info->powertrainConveyor->powertrainEmpty);
        if(node->trainType == HybridEngine){
            sem_post(info->powertrainConveyor->hybridEmpty);
        }

        nanosleep(sleepTime, NULL);

        sem_wait(info->poweredChassisConveyor->chassisEmpty);
        sem_wait(info->poweredChassisConveyor->chassisMutex);

        char str[50];
        strcpy(str, powertrain_consumerNames[MegaForce]);
        strcat(str, powertrain_producerNames[node->trainType]);

        Node* chassisNode = newNode(str, node->trainType, MegaForce);
        enqueue(info->poweredChassisConveyor->poweredChassisQueue, chassisNode);
        info->poweredChassisConveyor->produced++;
        
        log_added_poweredchassis(chassisNode->string, info->poweredChassisConveyor->poweredChassisQueue->length);
        free(node);
        sem_post(info->poweredChassisConveyor->chassisMutex);
        sem_post(info->poweredChassisConveyor->chassisEmpty);
    }
}

void* roboMountRobot(void *ptr){
    PoweredChassisConsumer* info = (PoweredChassisConsumer*)ptr;
    struct timespec sleepTime = {0, info->sleep * 1000000};
    while(true){
        sem_wait(info->poweredChassisConveyor->chassisFull);
        sem_wait(info->poweredChassisConveyor->chassisMutex);

        Node* node = dequeue(info->poweredChassisConveyor->poweredChassisQueue);
        info->poweredChassisConveyor->consumed[node->chassisType][node->trainType]++;
        
        int totalConsumed = 0;
        for(int i = 0; i < ChassisRobotTypeN; i++){
            for(int j = 0; j < PowertrainTypeN; j++){
                totalConsumed += info->poweredChassisConveyor->consumed[i][j];
            }
        }

        log_removed_poweredchassis(node->string, info->poweredChassisConveyor->poweredChassisQueue->length, totalConsumed);
        free(node);

        sem_post(info->poweredChassisConveyor->chassisMutex);
        sem_post(info->poweredChassisConveyor->chassisEmpty);

        if(totalConsumed == info->numToProduce){
            break;
        }
        nanosleep(sleepTime, NULL);
    }
    sem_post(info->barrier);
}