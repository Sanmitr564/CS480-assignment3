#include <stdbool.h>
#include <time.h>
#include <stdlib.h>
#include <string.h>

#include "car_assemble.h"
#include "carProduction.h"
#include "log.h"

#define MILLI_TO_NANO 1000000

/**
 * @brief thread for producing gas engines
 * 
 * @param ptr expects a pointer to PowertrainProducer
 * 
 * @return NULL
 */
void* gasEngine(void *ptr){
    //casts ptr to PowertrainProducer*
    PowertrainProducer* info = (PowertrainProducer*)ptr;

    //creates timespec for sleep
    const struct timespec sleepTime = {0, info->sleep * MILLI_TO_NANO};

    //production loop
    while(true){
        //simulate production
        nanosleep(&sleepTime, NULL);
        
        //wait for space and queue access
        sem_wait(info->powertrainConveyor->powertrainEmpty);
        sem_wait(info->powertrainConveyor->powertrainMutex);
        
        //check total produced
        //if equals total, return queue access and stop production
        //else continue production
        int produced = 0;
        for(int i = 0; i < PowertrainTypeN; i++){
            produced += info->powertrainConveyor->produced[i];
        }
        if(info->numToProduce == produced){
            sem_post(info->powertrainConveyor->powertrainMutex);
            break; 
        }
        
        //create node and put it into queue
        char str[NODE_STR_LEN];
        strcpy(str, powertrain_producerNames[GasEngine]);
        Node* node = newNode(str, GasEngine, 0);
        enqueue(info->powertrainConveyor->powertrainQueue, node);

        //increment counter
        info->powertrainConveyor->produced[GasEngine]++;
        
        //create PowertrainAdded to feed to log_added_powertrain
        unsigned int* inAssemblyQueue = (unsigned int*)malloc(PowertrainTypeN * sizeof(int));
        for(int i = 0; i < PowertrainTypeN; i++){
            inAssemblyQueue[i] = info->powertrainConveyor->produced[i] - info->powertrainConveyor->consumed[i];
        }
        PowertrainAdded powertrain = {GasEngine, info->powertrainConveyor->produced, inAssemblyQueue};
        log_added_powertrain(powertrain);
        free(inAssemblyQueue);

        //exit critical area
        sem_post(info->powertrainConveyor->powertrainMutex);
        sem_post(info->powertrainConveyor->powertrainFull);
    }
    return NULL;
}

/**
 * @brief thread for producing hybrid engines
 * 
 * @param ptr expects a pointer to PowertrainProducer
 * 
 * @return NULL
 */
void* hybridEngine(void *ptr){
    //cast ptr to PowertrainProducer*
    PowertrainProducer* info = (PowertrainProducer*)ptr;

    //creates timespec for sleep
    const struct timespec sleepTime = {0, info->sleep * MILLI_TO_NANO};

    //production loop
    while(true){
        //simulate production
        nanosleep(&sleepTime, NULL);

        //wait for hybrid availability, space, and queue access
        sem_wait(info->powertrainConveyor->hybridEmpty);
        sem_wait(info->powertrainConveyor->powertrainEmpty);
        sem_wait(info->powertrainConveyor->powertrainMutex);

        //check total produced
        //if equals total, return queue access and stop production
        //else continue production
        int produced = 0;
        for(int i = 0; i < PowertrainTypeN; i++){
            produced += info->powertrainConveyor->produced[i];
        }
        if(info->numToProduce == produced){
            sem_post(info->powertrainConveyor->powertrainMutex);
            break; 
        }
        
        //create node and put it into queue
        char str[NODE_STR_LEN];
        strcpy(str, powertrain_producerNames[HybridEngine]);
        Node* node = newNode(str, HybridEngine, 0);
        enqueue(info->powertrainConveyor->powertrainQueue, node);

        //increment counter
        info->powertrainConveyor->produced[HybridEngine]++;
        
        //create PowertrainAdded to feed to log_added_powertrain
        unsigned int* inAssemblyQueue = (unsigned int*)malloc(PowertrainTypeN * sizeof(int));
        for(int i = 0; i < PowertrainTypeN; i++){
            inAssemblyQueue[i] = info->powertrainConveyor->produced[i] - info->powertrainConveyor->consumed[i];
        }
        PowertrainAdded powertrain = {HybridEngine, info->powertrainConveyor->produced, inAssemblyQueue};
        log_added_powertrain(powertrain);
        free(inAssemblyQueue);

        //exit critical area
        sem_post(info->powertrainConveyor->powertrainMutex);
        sem_post(info->powertrainConveyor->powertrainFull);
    }
    return NULL;
}

/**
 * @brief thread for titano robot
 * 
 * @param ptr expects a pointer to PoweredChassisProducer
 * 
 * @return NULL 
 */
void* titanoRobot(void* ptr){
    //cast ptr to PoweredChassisProducer*
    PoweredChassisProducer* info = (PoweredChassisProducer*)ptr;

    //creates timespec for sleep
    const struct timespec sleepTime = {0, info->sleep * MILLI_TO_NANO};

    //consumption/production loop
    while(true){
        //wait for queue to have members and access
        sem_wait(info->powertrainConveyor->powertrainFull);
        sem_wait(info->powertrainConveyor->powertrainMutex);

        //check total consumed
        //if equals numToProduce exit
        //else continue production
        int consumed = 0;
        for(int i = 0; i < PowertrainTypeN; i++){
            consumed += info->powertrainConveyor->consumed[i];
        }
        if(info->numToProduce == consumed){
            sem_post(info->powertrainConveyor->powertrainMutex);
            break;
        }

        //remove node from powertrain queue
        Node* node = dequeue(info->powertrainConveyor->powertrainQueue);

        //increment consumed counter
        info->powertrainConveyor->consumed[node->trainType]++;

        //create PowertrainRemoved to feed log_removed_powertrain
        unsigned int* inAssemblyQueue = (unsigned int*)malloc(PowertrainTypeN * sizeof(int));
        for(int i = 0; i < PowertrainTypeN; i++){
            inAssemblyQueue[i] = info->powertrainConveyor->produced[i] - info->powertrainConveyor->consumed[i];
        }
        PowertrainRemoved powertrain = {Titano, node->trainType, info->powertrainConveyor->consumed, inAssemblyQueue};
        log_removed_powertrain(powertrain);
        free(inAssemblyQueue);

        //return queue access and increment available queue slots
        sem_post(info->powertrainConveyor->powertrainMutex);
        sem_post(info->powertrainConveyor->powertrainEmpty);

        //if HybridEngine was removed, increment available hybrid slots in queue
        if(node->trainType == HybridEngine){
            sem_post(info->powertrainConveyor->hybridEmpty);
        }

        //sleep to simulate production
        nanosleep(&sleepTime, NULL);

        //wait for space and queue access
        sem_wait(info->poweredChassisConveyor->chassisEmpty);
        sem_wait(info->poweredChassisConveyor->chassisMutex);

        //create and add node to queue
        char str[NODE_STR_LEN];
        strcpy(str, powertrain_consumerNames[Titano]);
        strcat(str, powertrain_producerNames[node->trainType]);
        Node* chassisNode = newNode(str, node->trainType, Titano);
        enqueue(info->poweredChassisConveyor->poweredChassisQueue, chassisNode);

        //increment produced powered chassis counter
        info->poweredChassisConveyor->produced++;
        
        //run log
        log_added_poweredchassis(chassisNode->string, info->poweredChassisConveyor->poweredChassisQueue->length);
        
        //free removed node
        free(node);

        //exit critical region
        sem_post(info->poweredChassisConveyor->chassisMutex);
        sem_post(info->poweredChassisConveyor->chassisFull);
    }
    return NULL;
}

/**
 * @brief thread for megaforce robot
 * 
 * @param ptr expects a pointer to PoweredChassisProducer
 * 
 * @return NULL
 */
void* megaForceRobot(void *ptr){
    //cast ptr to PoweredChassisProducer*
    PoweredChassisProducer* info = (PoweredChassisProducer*)ptr;

    //create timespec for sleep
    const struct timespec sleepTime = {0, info->sleep * MILLI_TO_NANO};

    //consumption/production loop
    while(true){
        //wait for queue to have members and access
        sem_wait(info->powertrainConveyor->powertrainFull);
        sem_wait(info->powertrainConveyor->powertrainMutex);
        
        //check total consumed
        //if equals numToProduce exit
        //else continue production
        int consumed = 0;
        for(int i = 0; i < PowertrainTypeN; i++){
            consumed += info->powertrainConveyor->consumed[i];
        }
        if(info->numToProduce == consumed){
            sem_post(info->powertrainConveyor->powertrainMutex);
            break;
        }

        //remove node from powertrain queue
        Node* node = dequeue(info->powertrainConveyor->powertrainQueue);

        //increment consumed counter
        info->powertrainConveyor->consumed[node->trainType]++;

        //create PowertrainRemoved to feed log_removed_powertrain
        unsigned int* inAssemblyQueue = (unsigned int*)malloc(PowertrainTypeN * sizeof(int));
        for(int i = 0; i < PowertrainTypeN; i++){
            inAssemblyQueue[i] = info->powertrainConveyor->produced[i] - info->powertrainConveyor->consumed[i];
        }
        PowertrainRemoved powertrain = {MegaForce, node->trainType, info->powertrainConveyor->consumed, inAssemblyQueue};
        log_removed_powertrain(powertrain);
        free(inAssemblyQueue);

        //return queue access and increment available queue slots
        sem_post(info->powertrainConveyor->powertrainMutex);
        sem_post(info->powertrainConveyor->powertrainEmpty);

        //if HybridEngine was removed, increment available hybrid slots in queue
        if(node->trainType == HybridEngine){
            sem_post(info->powertrainConveyor->hybridEmpty);
        }

        //sleep to simulate production
        nanosleep(&sleepTime, NULL);

        //wait for space and queue access
        sem_wait(info->poweredChassisConveyor->chassisEmpty);
        sem_wait(info->poweredChassisConveyor->chassisMutex);

        //create and add node to queue
        char str[NODE_STR_LEN];
        strcpy(str, powertrain_consumerNames[MegaForce]);
        strcat(str, powertrain_producerNames[node->trainType]);
        Node* chassisNode = newNode(str, node->trainType, MegaForce);
        enqueue(info->poweredChassisConveyor->poweredChassisQueue, chassisNode);

        //increment produced powered chassis counter
        info->poweredChassisConveyor->produced++;
        
        //run log
        log_added_poweredchassis(chassisNode->string, info->poweredChassisConveyor->poweredChassisQueue->length);

        //free removed node
        free(node);

        
        sem_post(info->poweredChassisConveyor->chassisMutex);
        sem_post(info->poweredChassisConveyor->chassisFull);
    }
    return NULL;
}

/**
 * @brief thread for robomount robot
 * 
 * @param ptr expects a pointer to PoweredChassisConsumer
 * 
 * @return NULL
 */
void* roboMountRobot(void *ptr){
    //cast ptr to PoweredChassisConsumer*
    PoweredChassisConsumer* info = (PoweredChassisConsumer*)ptr;

    //create timespec for sleep
    const struct timespec sleepTime = {0, info->sleep * MILLI_TO_NANO};

    //consumption loop
    while(true){

        //wait for queue to have members and access
        sem_wait(info->poweredChassisConveyor->chassisFull);
        sem_wait(info->poweredChassisConveyor->chassisMutex);

        //remove node from queue
        Node* node = dequeue(info->poweredChassisConveyor->poweredChassisQueue);

        //increment consumed powered chassis counter
        info->poweredChassisConveyor->consumed[node->chassisType][node->trainType]++;
        
        //calculate total consumed to use later
        int totalConsumed = 0;
        for(int i = 0; i < ChassisRobotTypeN; i++){
            for(int j = 0; j < PowertrainTypeN; j++){
                totalConsumed += info->poweredChassisConveyor->consumed[i][j];
            }
        }

        //log removed chassis
        char str[NODE_STR_LEN];
        strcpy(str, node->string);
        log_removed_poweredchassis(str, info->poweredChassisConveyor->poweredChassisQueue->length, totalConsumed);
        
        //free removed node
        free(node);

        //exit critical region
        sem_post(info->poweredChassisConveyor->chassisMutex);
        sem_post(info->poweredChassisConveyor->chassisEmpty);

        //if total consumed == numToProduce leave loop
        //else continue loop
        if(totalConsumed == info->numToProduce){
            break;
        }

        //sleep to simulate production
        nanosleep(&sleepTime, NULL);
    }
    //free main thread
    sem_post(info->barrier);
    return NULL;
}