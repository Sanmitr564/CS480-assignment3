#include <string.h>
#include <stdlib.h>
#include "queue.h"
#include "car_assemble.h"


int enqueue(Queue* queue, Node* node){
    if(queue->head == NULL){
        queue->head = node;
    }else{
        queue->tail->next = node;
    }
    queue->length++;
}

Node* dequeue(Queue* queue){
    if(queue->length == 0){
        return NULL;
    }
    Node* toRemove = queue->head;
    queue->head = toRemove->next;

    queue->length--;
    return toRemove;
}

Node* newNode(char *str, PowertrainType trainType, ChassisRobotType chassisType){
    if(strlen(str) > 50){
        return -1;
    }

    Node* newNode = (Node*)(malloc(sizeof(Node)));
    strcpy(newNode->string, str);
    newNode->trainType = trainType;
    newNode->chassisType = chassisType;

    return newNode;
}

Queue* newQueue(){
    Queue* queue = (Queue*)(malloc(sizeof(Queue)));
    queue->head = NULL;
    queue->tail = NULL;
    queue->length = 0;

    return queue;
}