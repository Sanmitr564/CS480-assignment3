#include <semaphore.h>

#include "queue.h"

//shared data structure for robots with access to the powertrain conveyor
struct PowertrainConveyor{
    Queue* powertrainQueue;     //fifo queue to simulate conveyor

    sem_t* powertrainMutex;     //
    sem_t* powertrainEmpty;     //semaphores to regulate
    sem_t* powertrainFull;      //access to the queue
    sem_t* hybridEmpty;         //

    unsigned int* produced;     //arrays of len PowertrainTypeN to track what types
    unsigned int* consumed;     //of engines have been produced and consumed
} typedef PowertrainConveyor;

//personal data structure for robots that produce powertrains
struct PowertrainProducer{
    PowertrainConveyor* powertrainConveyor; //shared powertrain conveyor

    int numToProduce;                       //total number to produce

    int sleep;                              //milliseconds to sleep to simulate production time
} typedef PowertrainProducer;

//shared data structure for robots with access to the powered chassis conveyor
struct PoweredChassisConveyor{
    Queue* poweredChassisQueue;     //fifo queue to simulate conveyor

    sem_t* chassisMutex;            //semaphores to regulate
    sem_t* chassisEmpty;            //access to the queue
    sem_t* chassisFull;             //

    unsigned int produced;          //total number of powered chassis created

    unsigned int **consumed;        //2d array of len [ChassisRobotTypeN][PowertrainTypeN] to track what kinds of chassis have been produced
} typedef PoweredChassisConveyor;

//personal data structure for robots that consume powered chassis
struct PoweredChassisConsumer{
    PoweredChassisConveyor* poweredChassisConveyor; //shared powered chassis conveyor

    int numToProduce;                               //total number to produce

    sem_t* barrier;                                 //barrier semaphore to regulate main thread flow

    int sleep;                                      //milliseconds to sleep to simulate production time
} typedef PoweredChassisConsumer;

//personal data structure for robots that produce powered chassis
struct PoweredChassisProducer{
    PowertrainConveyor* powertrainConveyor;         //shared powertrain conveyor

    PoweredChassisConveyor* poweredChassisConveyor; //shared powered chassis conveyor

    int numToProduce;                               //total number to produce

    int sleep;                                      //milliseconds to sleep to simulate production time
} typedef PoweredChassisProducer;

//queue restraints as per assignment specifications
typedef enum QueueRestraints{
    PowertrainConveyorMax = 20,
    HybridMax = 8,
    PoweredChassisConveyorMax = 15
} QueueRestraints;

/**
 * @brief initializes new PowertrainConveyor and fields to be shared
 * 
 * @return initialized PowertrainConveyor* 
 */
PowertrainConveyor* newPowertrainConveyor();

/**
 * @brief initializes new PoweredChassisConveyor and fields to be shared
 * 
 * @return initialized PoweredChassisConveyor* 
 */
PoweredChassisConveyor* newPoweredChassisConveyor();

/**
 * @brief initializes new robot specific PowertrainProducer struct and fields
 * 
 * @param conveyor shared PowertrainConveyor robot has access to
 * @param numToProduce how many total powertrains the robots need to produce
 * @param sleep how long to sleep in milliseconds
 * 
 * @return initialized PowertrainProducer* 
 */
PowertrainProducer* newPowertrainProducer(PowertrainConveyor* conveyor, int numToProduce, unsigned int sleep);

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
PoweredChassisProducer* newPoweredChassisProducer(PowertrainConveyor* powerTrainConveyor, PoweredChassisConveyor* poweredChassisConveyer, int numToProduce, unsigned int sleep);

/**
 * @brief initializes new robot specific PoweredChassisConsumer struct and fields
 * 
 * @param conveyor shared PoweredChassisConveyor robot has access to
 * @param numToProduce how many total powered chassis the robots need to produce
 * @param sleep how long to sleep in milliseconds
 * @return initialized PoweredChassisConsumer* 
 */
PoweredChassisConsumer* newPoweredChassisConsumer(PoweredChassisConveyor* conveyor, int numToProduce, unsigned int sleep);