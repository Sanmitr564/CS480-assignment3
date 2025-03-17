#include <stdio.h>
#include <time.h>
#include <pthread.h>
#include <semaphore.h>

#include "log.h"

/*
 * i/o functions - assumed to be called in a critical section
 */


/* Handle C++ namespaces, ignore if compiled in C 
 * C++ usually uses this #define to declare the C++ standard.
 * It will not be defined if a C compiler is used.
 */
#ifdef __cplusplus
using namespace std;
#endif

sem_t sem_print;  // Semaphore for ensuring mutex printing, 
                  // I know this is bad practice in terms of using Global, which is not ideal.
                  // Do NOT use the Global variables in your code! 

pthread_once_t init_once = PTHREAD_ONCE_INIT;  // Ensures one-time initialization

void initialize_semaphore() {
    sem_init(&sem_print, 0, 1);  // Initialize semaphore with value 1
}

/*
 * Data section - names must align with the enumerated types
 * defined in car_assemble.h
 */

/* Names of powertrain producer threads and powertrain types */
const char *powertrain_producers[] = {"GasEngine powertrain", "HybridEngine powertrain"};
const char *powertrain_producerNames[] = {"GAS", "HYBRID"};

/**
 * Names of poweredchassis threads, robots for marrying the powertrain to the chassis
 * Consumers of the powertrains for marrying the powertrain to the chassis
 * Also producers of powered chassis (powertrain installed onto chassis) to the body installer
 */
const char *powertrain_consumerNames[] = {"Titano", "MegaForce"};

/**
 * Name of the body installer robot consumer
 * Consumer of the powered chassis, to install powered chassis to the car body 
 **/
const char *poweredchassis_consumerName = "RoboMount";

/* double elapsed_s()
 * show seconds of wall clock time used by the process
 */

double elapsed_s() {
  const double ns_per_s = 1e9; /* nanoseconds per second */

  /* Initialize the first time we call this */
  static timespec start;
  static int firsttime = 1;

  struct timespec t;

  /* get elapsed wall clock time for this process
   * note:  use CLOCK_PROCESS_CPUTIME_ID for CPU time (not relevant here
   * as we will be sleeping a lot on the semaphores)
   */
  clock_gettime(CLOCK_REALTIME, &t);

  if (firsttime) {
    /* first time we've called the function, store the current
     * time.  This will not track the cost of the first item
     * produced, but is a reasonable approximation for the
     * whole process and avoids having to create an initialization
     * function.
     * (In C++, we'd just build a timer object and pass it around,
     *  but this approximation provides a simple interface for both
     *  C and C++.)
     */
    firsttime = 0;  /* don't do this again */
    start = t;  /* note when we started */
  }
  
  /* determine time delta from start and convert to s */
  double s = (t.tv_sec - start.tv_sec) + 
    (t.tv_nsec - start.tv_nsec) / ns_per_s ;
  return s;
}

/**
 * @brief Show that a powertrain hs of the powertrain queue.
 * 
 * @param powertrainAdded    as been added to the powertrain queue and 
 *        print the current statu  see header for this type
 * 
 * produced and inAssemblyQueue reflect numbers *after* adding the current powertrain.
 */
void log_added_powertrain(PowertrainAdded powertrainAdded) {
  int idx;
  int total;

  pthread_once(&init_once, initialize_semaphore);  // Ensure one-time initialization

  // lock for mutex printing
  sem_wait(&sem_print);

  /* Show what is in the powertrain queue */
  printf("Powertrain_queue: ");
  total = 0;  /* total produced */
  for (idx=0; idx < PowertrainTypeN; idx++) {
    if (idx > 0)
      printf(" + ");  /* separator */
    printf("%d %s", powertrainAdded.inAssemblyQueue[idx], powertrain_producerNames[idx]);
    total += powertrainAdded.inAssemblyQueue[idx];
  }

  printf(" = %d. ", total);

  printf("Added %s.", powertrain_producers[powertrainAdded.type]);

  /* Show what has been produced */
  total = 0;
  printf(" Produced: ");
  for (idx=0; idx < PowertrainTypeN; idx++) {
    total += powertrainAdded.produced[idx];  /* track total produced */
    if (idx > 0)
      printf(" + ");  /* separator */
    printf("%d %s", powertrainAdded.produced[idx], powertrain_producerNames[idx]);
  }
  /* total produced over how long */
  printf(" = %d in %.3f s.\n", total, elapsed_s());
  //printf(" = %d\n", total);

  /* This is not really needed, but will be helpful for making sure that you
   * see output prior to a segmentation vioilation.  This is not usually a
   * good practice as we want to avoid ending the CPU burst premaurely which
   * this will do, but it is a helpful technique.
   */
  fflush(stdout);  

  sem_post(&sem_print); // release the semaphore for printing
};

/**
 * @brief   Show that a powertrain has been removed from the powertrain queue 
 *          and print the current status of the powertrain queue.
 *
 * @param powertrainRemoved    see header for this type
 * 
 * Counts reflect numbers *after* the powertrain has been removed
 */
void log_removed_powertrain(PowertrainRemoved powertrainRemoved) {
  int idx;
  int total;

  pthread_once(&init_once, initialize_semaphore);  // Ensure one-time initialization

  // lock for mutex printing
  sem_wait(&sem_print);

  /* Show what is in the powertrain queue */
  total = 0;
  printf("Powertrain_queue: ");
  for (idx=0; idx < PowertrainTypeN; idx++) {
    if (idx > 0)
      printf(" + ");  /* separator */
    printf("%d %s", powertrainRemoved.inAssemblyQueue[idx], powertrain_producerNames[idx]);
    total += powertrainRemoved.inAssemblyQueue[idx];
  }
  printf(" = %d. ", total);
  
  /* Show what has been consumed by consumer */
  printf("%s consumed %s.  %s totals: ",
    powertrain_consumerNames[powertrainRemoved.consumer],
    powertrain_producers[powertrainRemoved.type],
    powertrain_consumerNames[powertrainRemoved.consumer]);
  total = 0;
  for (idx = 0; idx < PowertrainTypeN; idx++) {
    if (idx > 0)
      printf(" + ");  /* separator */
    total += powertrainRemoved.consumed[idx];  /* track total consumed */
    printf("%d %s", powertrainRemoved.consumed[idx], powertrain_producerNames[idx]);
  }
  /* total consumed over how long */
  printf(" = %d consumed in %.3f s.\n", total, elapsed_s());
  //printf(" = %d consumed\n", total);

  /* This is not really needed, but will be helpful for making sure that you
   * see output prior to a segmentation vioilation.  This is not usually a
   * good practice as we want to avoid ending the CPU burst premaurely which
   * this will do, but it is a helpful technique.
   */
  fflush(stdout);

  sem_post(&sem_print); // release the semaphore for printing
};

/**
 * @brief   Show that an item has been added to the powered chassis queue 
 *          and print the current size of the powered chassis queue.
 *
 * @param poweredChassisAdded   a string combining powertrain consumer name and 
 *                              powertrain name, e.g., TitanoHYBRID, see log.c
 * @param powerChassisQueueSize Size *after* the chassis has been added
 */
void log_added_poweredchassis(const char* poweredChassisAdded,
                              unsigned int poweredChassisQueueSize) {

  pthread_once(&init_once, initialize_semaphore);  // Ensure one-time initialization

  // lock for mutex printing
  sem_wait(&sem_print);

  /* Show what is in the poweredchassis queue */
  printf("Poweredchassis_queue: produced and added %s in %.3f s, queue size: %d\n", 
         poweredChassisAdded, elapsed_s(), poweredChassisQueueSize);

  sem_post(&sem_print); // release the semaphore for printing
}

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
                                unsigned int totalConsumed) {

  pthread_once(&init_once, initialize_semaphore);  // Ensure one-time initialization

  // lock for mutex printing
  sem_wait(&sem_print);

  /* Show what is in the poweredchassis queue */
  printf("Poweredchassis_queue: removed and consumed %s in %.3f s, queue size: %d, total consumed: %d\n", 
    poweredChassisRemoved, elapsed_s(), poweredChassisQueueSize, totalConsumed);

  sem_post(&sem_print); // release the semaphore for printing
}

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
void log_powertrain_history(unsigned int produced[], 
                            unsigned int *consumed[]) {
  int p, c;  /* array indices */
  int total;

  printf("\nREQUEST REPORT\n----------------------------------------\n");
   
  /* show number produced for each producer / request type */
  for (p = 0; p < PowertrainTypeN; p++) {
    printf("%s producer generated %d requests\n",
	   powertrain_producers[p], produced[p]);
  }
  /* show number consumed by each consumer */
  for (c=0; c < ChassisRobotTypeN; c++) {
    printf("%s consumed ", powertrain_consumerNames[c]);
    total = 0;
    for (p = 0; p < PowertrainTypeN; p++) {
      if (p > 0)
	printf(" + ");
    
      total += consumed[c][p];
      printf("%d %s", consumed[c][p], powertrain_producerNames[p]);
    }
    printf(" = %d total\n", total);
  }

  printf("Elapsed time %.3f s\n", elapsed_s());
}
