#include "car_assemble.h"

#define NODE_STR_LEN 50

typedef struct Node {
    char string[NODE_STR_LEN];      //string to hold poweredchassis name

    PowertrainType trainType;       //type of powertrain and
    ChassisRobotType chassisType;   //chassis of the car

    struct Node* next;              //next node
} Node;

typedef struct Queue {
    Node* head; //queue head
    Node* tail; //queue tail
    int length; // queue length
} Queue;

/**
 * @brief adds node to queue
 * 
 * @param queue queue to add to
 * @param node node to add
 */
void enqueue(Queue* queue, Node* node);

/**
 * @brief removes node from queue
 *        still needs to be freed
 * 
 * @param queue queue to remove from
 * @return Node* removed node
 */
Node* dequeue(Queue* queue);

/**
 * @brief creates new node with params
 * 
 * @param str powered chassis name
 * @param trainType type of powertrain
 * @param chassisType type of chassis
 * @return Node* created node
 */
Node* newNode(char *str, PowertrainType trainType, ChassisRobotType chassisType);

/**
 * @brief initializes new queue
 * 
 * @return Queue* initialized queue
 */
Queue* newQueue();