#ifndef LOG_H
#define LOG_H

#include "car_assemble.h"

/**
 * @brief Show that a powertrain has been added to the powertrain queue and 
 *        print the current status of the powertrain queue.
 * 
 * @param powertrainAdded      see header for this type
 * 
 * produced and inAssemblyQueue reflect numbers *after* adding the current powertrain.
 */
void log_added_powertrain (PowertrainAdded powertrainAdded);

/**
 * @brief   Show that a powertrain has been removed from the powertrain queue 
 *          and print the current status of the powertrain queue.
 *
 * @param powertrainRemoved    see header for this type
 * 
 * Counts reflect numbers *after* the powertrain has been removed
 */
void log_removed_powertrain(PowertrainRemoved powertrainRemoved);

/**
 * @brief   Show that an item has been added to the powered chassis queue 
 *          and print the current size of the powered chassis queue.
 *
 * @param poweredChassisAdded   a string combining powertrain consumer name and 
 *                              powertrain name, e.g., TitanoHYBRID, see log.c
 * @param powerChassisQueueSize Size *after* the chassis has been added
 */
void log_added_poweredchassis(const char* poweredChassisAdded,
                              unsigned int poweredChassisQueueSize);

/**
 * @brief   Show that an item has been removed from the powered chassis queue 
 *          and print the current size of the powered chassis queue.
 *
 * @param poweredChassisRemoved a string combining powertrain consumer name and 
 *                              powertrain name, e.g., TitanoHYBRID, see log.c
 * @param powerChassisQueueSize Size *after* the chassis has been removed
 * @param totalConsumed         total number of consumed powered chassis so far
 */
void log_removed_poweredchassis(const char* poweredChassisRemoved,
                                unsigned int poweredChassisQueueSize,
                                unsigned int totalConsumed);

/**
 * @brief   Show how many powertrain of each type produced.  
 *          Show how many powertrain consumed by each consumer.
 * 
 * @param produced   count for each PowertrainType produced
 * @param consumed   array of pointers to consumed arrays for each consumer
 *                   e.g. consumed[Titano] points to an array that 
 *                   is indexed by request type
 *                   (it is an 2-D array, consumed[Titano][GasEngine] is 
 *                    the number for pizza delivery requests that were 
 *                    delivered by delivery service A)
 */
void log_powertrain_history(unsigned int produced[], unsigned int *consumed[]);

#endif
