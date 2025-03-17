#include <string.h>
#include <stdlib.h>

#include "queue.h"
#include "car_assemble.h"

/**
 * @brief adds node to queue
 * 
 * @param queue queue to add to
 * @param node node to add
 */
void enqueue(Queue* queue, Node* node){
    if(queue->head == NULL){        //if queue is empty, replace head and tail with node
        queue->head = node;
        queue->tail = node;
    }else{                          //else set tail->next to node and set tail to node
        queue->tail->next = node;
        queue->tail = node;
    }
    queue->length++;                //increment queue length
}

Node* dequeue(Queue* queue){
    if(queue->length == 0){         //return nothing if queue is empty
        return NULL;
    }

    Node* toRemove = queue->head;   //get front of queue

    queue->head = toRemove->next;   //set front of queue to second in line

    toRemove->next = NULL;          //prevent memory issues

    queue->length--;                //decrement length

    return toRemove;                //return node
}

/**
 * @brief creates new node with params
 * 
 * @param str powered chassis name
 * @param trainType type of powertrain
 * @param chassisType type of chassis
 * @return Node* created node
 */
Node* newNode(char *str, PowertrainType trainType, ChassisRobotType chassisType){
    if(strlen(str) > NODE_STR_LEN){                 //return null if str is too long
        return NULL;
    }

    Node* newNode = (Node*)(malloc(sizeof(Node)));  //create new node

    //populate fields
    strcpy(newNode->string, str);
    newNode->trainType = trainType;
    newNode->chassisType = chassisType;

    //return node
    return newNode;
}


Queue* newQueue(){
    //create new queue
    Queue* queue = (Queue*)(malloc(sizeof(Queue)));

    //populate fields
    queue->head = NULL;
    queue->tail = NULL;
    queue->length = 0;

    //return queue
    return queue;
}