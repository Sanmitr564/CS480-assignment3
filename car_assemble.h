#ifndef CARASSEMBLE_H
#define CARASSEMBLE_H

/*
 * Arrays with powertrain producer and powertrain consumer names
 * These can be indexed with the enumerated types below
 * and are defined in log.c
 */

 /* Names of powertrain producers (two robots) and powertrain types */
extern const char *powertrain_producers[];
extern const char *powertrain_producerNames[];

/**
 * Names of powertrain consumers (two robots) for marrying the powertrain to the chassis,
 * that produce the powered chassis. So they are also the producers of
 * the married powered chassises that will be consumed by the consumer of
 * powered chassis (boday installer) in the next assembling stage.
 * 
 * Consumers of the powertrains for marrying the powertrain to the chassis
 * Also producers of powered chassis (powertrain installed onto chassis), the body installer
 */
extern const char *powertrain_consumerNames[];

/**
 * Name of the consumer of the powered chassis (body installer), one robot 
 * that installs powered chassis to the car body 
 **/
extern const char *poweredchassis_consumerName;

/*
 * Enumerated types to be used by the producers and consumers
 * These are expected in the input/output functions (log.h)
 * should be useful in your producer and consumer code.
 */

/**
 * The powertrainQueue can hold up to a maximum of 20 unconsumed powertrains 
 * in its queue at any given time.
 *  
 * there can be no more than 8 hybrid engine power train  
 * in the powertrain queue at any given time. 
*/

/*
 * Two producers of powertrains (robots) are offered: 
 *   one robot for producing a Gas Engine powertrain, 
 *   one robot for producing a Hybrid Engine powertrain.
 * Each powertrain producer only produces one type of power trains,
 * so PowertrainType is synomonous with the producer type
 */
typedef enum Powertrains {
  GasEngine = 0,          // GasEngine powertrain
  HybridEngine = 1,       // HybridEngine powertrain 
  PowertrainTypeN = 2,   // number of powertrain types
} PowertrainType;

/* 
 * Two consumers of powertrains (robots) are marrying 
 * the powertrains to the chassis
 * These robots take the powertrains off from the powertrain queue 
 * in the order that they are produced and inserted to the queue (FIFO).
*/
typedef enum ChassisRobots {
  Titano = 0,   // Titano robot  
  MegaForce = 1,   // Megaforce robot
  ChassisRobotTypeN = 2, // Number of powertrain consumers
} ChassisRobotType;


typedef struct {
  // What kind of powertrain was produced?
  PowertrainType type; 
  // Array of number of powertrain of each type that have been produced
  unsigned int *produced; 
  // Array of number of powertrains of each type that are
  // in the powertrain queue and have not yet been consumed.
  // (inAssemblyQueue[GasEngine] and inAssemblyQueue[HybridEngine])
  unsigned int *inAssemblyQueue;
} PowertrainAdded;

typedef struct {
  // Who removed and processed the powertrain?
  ChassisRobotType consumer; 
  // What kind of powertrain was removed?  As each consumer consumes
  // one type of request, this is the same as the producer.
  PowertrainType type; 
  // Array of number of powertrains of each type that have been
  // consumed.
  unsigned int *consumed; 
  // Array of number of powertrains of each type that are
  // in the powertrain queue and have not yet been consumed.
  // (inAssemblyQueue[GasEngine] and inAssemblyQueue[HybridEngine])
  unsigned int *inAssemblyQueue;
} PowertrainRemoved;

#endif
