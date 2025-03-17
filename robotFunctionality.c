#include <stdbool.h>
#include <time.h>
#include <stdlib.h>
#include <string.h>

#include "car_assemble.h"
#include "carProduction.h"
#include "log.h"

#define MILLI_TO_NANO 1000000

void* gasEngine(void *ptr){
    PowertrainProducer* info = (PowertrainProducer*)ptr;
    const struct timespec sleepTime = {0, info->sleep * MILLI_TO_NANO};
    while(true){
        nanosleep(&sleepTime, NULL);

        sem_wait(info->powertrainConveyor->powertrainEmpty);
        sem_wait(info->powertrainConveyor->powertrainMutex);

        int produced = 0;
        for(int i = 0; i < PowertrainTypeN; i++){
            produced += info->powertrainConveyor->produced[i];
        }
        if(info->numToProduce == produced){
            sem_post(info->powertrainConveyor->powertrainMutex);
            break; 
        }
        
        char str[NODE_STR_LEN];
        strcpy(str, powertrain_producerNames[GasEngine]);

        Node* node = newNode(str, GasEngine, 0);
        enqueue(info->powertrainConveyor->powertrainQueue, node);
        info->powertrainConveyor->produced[GasEngine]++;
        
        unsigned int* inAssemblyQueue = (unsigned int*)malloc(PowertrainTypeN * sizeof(int));
        for(int i = 0; i < PowertrainTypeN; i++){
            inAssemblyQueue[i] = info->powertrainConveyor->produced[i] - info->powertrainConveyor->consumed[i];
        }
        PowertrainAdded powertrain = {GasEngine, info->powertrainConveyor->produced, inAssemblyQueue};
        log_added_powertrain(powertrain);
        free(inAssemblyQueue);

        sem_post(info->powertrainConveyor->powertrainMutex);
        sem_post(info->powertrainConveyor->powertrainFull);
    }
    return NULL;
}

void* hybridEngine(void *ptr){
    PowertrainProducer* info = (PowertrainProducer*)ptr;
    const struct timespec sleepTime = {0, info->sleep * MILLI_TO_NANO};
    while(true){
        nanosleep(&sleepTime, NULL);

        sem_wait(info->powertrainConveyor->hybridEmpty);
        sem_wait(info->powertrainConveyor->powertrainEmpty);
        sem_wait(info->powertrainConveyor->powertrainMutex);

        int produced = 0;
        for(int i = 0; i < PowertrainTypeN; i++){
            produced += info->powertrainConveyor->produced[i];
        }
        if(info->numToProduce == produced){
            sem_post(info->powertrainConveyor->powertrainMutex);
            break; 
        }

        char str[NODE_STR_LEN];
        strcpy(str, powertrain_producerNames[HybridEngine]);

        Node* node = newNode(str, HybridEngine, 0);
        enqueue(info->powertrainConveyor->powertrainQueue, node);
        info->powertrainConveyor->produced[HybridEngine]++;
        
        unsigned int* inAssemblyQueue = (unsigned int*)malloc(PowertrainTypeN * sizeof(int));
        for(int i = 0; i < PowertrainTypeN; i++){
            inAssemblyQueue[i] = info->powertrainConveyor->produced[i] - info->powertrainConveyor->consumed[i];
        }
        PowertrainAdded powertrain = {HybridEngine, info->powertrainConveyor->produced, inAssemblyQueue};
        log_added_powertrain(powertrain);
        free(inAssemblyQueue);

        sem_post(info->powertrainConveyor->powertrainMutex);
        sem_post(info->powertrainConveyor->powertrainFull);
    }
    return NULL;
}

void* titanoRobot(void* ptr){
    PoweredChassisProducer* info = (PoweredChassisProducer*)ptr;
    const struct timespec sleepTime = {0, info->sleep * MILLI_TO_NANO};
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
        info->powertrainConveyor->consumed[node->trainType]++;

        unsigned int* inAssemblyQueue = (unsigned int*)malloc(PowertrainTypeN * sizeof(int));
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

        nanosleep(&sleepTime, NULL);

        sem_wait(info->poweredChassisConveyor->chassisEmpty);
        sem_wait(info->poweredChassisConveyor->chassisMutex);

        char str[NODE_STR_LEN];
        strcpy(str, powertrain_consumerNames[Titano]);
        strcat(str, powertrain_producerNames[node->trainType]);

        Node* chassisNode = newNode(str, node->trainType, Titano);
        enqueue(info->poweredChassisConveyor->poweredChassisQueue, chassisNode);
        info->poweredChassisConveyor->produced++;
        
        log_added_poweredchassis(chassisNode->string, info->poweredChassisConveyor->poweredChassisQueue->length);
        free(node);
        sem_post(info->poweredChassisConveyor->chassisMutex);
        sem_post(info->poweredChassisConveyor->chassisFull);
    }
    return NULL;
}

void* megaForceRobot(void *ptr){
    PoweredChassisProducer* info = (PoweredChassisProducer*)ptr;
    const struct timespec sleepTime = {0, info->sleep * MILLI_TO_NANO};
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
        info->powertrainConveyor->consumed[node->trainType]++;

        unsigned int* inAssemblyQueue = (unsigned int*)malloc(PowertrainTypeN * sizeof(int));
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

        nanosleep(&sleepTime, NULL);

        sem_wait(info->poweredChassisConveyor->chassisEmpty);
        sem_wait(info->poweredChassisConveyor->chassisMutex);

        char str[NODE_STR_LEN];
        strcpy(str, powertrain_consumerNames[MegaForce]);
        strcat(str, powertrain_producerNames[node->trainType]);

        Node* chassisNode = newNode(str, node->trainType, MegaForce);
        enqueue(info->poweredChassisConveyor->poweredChassisQueue, chassisNode);
        info->poweredChassisConveyor->produced++;
        
        log_added_poweredchassis(chassisNode->string, info->poweredChassisConveyor->poweredChassisQueue->length);
        free(node);
        sem_post(info->poweredChassisConveyor->chassisMutex);
        sem_post(info->poweredChassisConveyor->chassisFull);
    }
    return NULL;
}

void* roboMountRobot(void *ptr){
    PoweredChassisConsumer* info = (PoweredChassisConsumer*)ptr;
    const struct timespec sleepTime = {0, info->sleep * MILLI_TO_NANO};
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

        char str[NODE_STR_LEN];
        strcpy(str, node->string);
        log_removed_poweredchassis(str, info->poweredChassisConveyor->poweredChassisQueue->length, totalConsumed);
        free(node);

        sem_post(info->poweredChassisConveyor->chassisMutex);
        sem_post(info->poweredChassisConveyor->chassisEmpty);

        if(totalConsumed == info->numToProduce){
            break;
        }
        nanosleep(&sleepTime, NULL);
    }
    sem_post(info->barrier);
    return NULL;
}