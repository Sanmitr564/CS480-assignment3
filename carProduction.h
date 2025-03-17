#include <semaphore.h>
#include "queue.h"

struct PowertrainConveyor{
    Queue* powertrainQueue;
    sem_t* powertrainMutex;
    sem_t* powertrainEmpty;
    sem_t* powertrainFull;
    sem_t* hybridEmpty;
    sem_t* hybridFull;

    unsigned int* produced;
    unsigned int* consumed;
} typedef PowertrainConveyor;

struct PowertrainProducer{
    PowertrainConveyor* powertrainConveyor;
    int numToProduce;
    int sleep;
} typedef PowertrainProducer;

struct PoweredChassisConveyor{
    Queue* poweredChassisQueue;
    sem_t* chassisMutex;
    sem_t* chassisEmpty;
    sem_t* chassisFull;

    unsigned int produced;
    unsigned int **consumed;
} typedef PoweredChassisConveyor;

struct PoweredChassisConsumer{
    PoweredChassisConveyor* poweredChassisConveyor;
    int numToProduce;
    sem_t* barrier;
    int sleep;
} typedef PoweredChassisConsumer;

struct PoweredChassisProducer{
    PowertrainConveyor* powertrainConveyor;
    PoweredChassisConveyor* poweredChassisConveyor;
    int numToProduce;
    int sleep;
} typedef PoweredChassisProducer;

typedef enum QueueRestraints{
    PowertrainConveyorMax = 20,
    HybridMax = 8,
    PoweredChassisConveyorMax = 15
} QueueRestraints;

PowertrainConveyor* newPowertrainConveyor();

PowertrainProducer* newPowertrainProducer(PowertrainConveyor* conveyor, int numToProduce, unsigned int sleep);

PoweredChassisConveyor* newPoweredChassisConveyor();

PoweredChassisConsumer* newPoweredChassisConsumer(PoweredChassisConveyor* conveyor, int numToProduce, unsigned int sleep);

PoweredChassisProducer* newPoweredChassisProducer(PowertrainConveyor* powerTrainConveyor, PoweredChassisConveyor* poweredChassisConveyer, int numToProduce, unsigned int sleep);

void* gasEngine(void *ptr);

void* hybridEngine(void *ptr);

void* titanoRobot(void *ptr);

void* megaForceRobot(void *ptr);

void* roboMountRobot(void *ptr);