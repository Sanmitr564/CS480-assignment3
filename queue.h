#include "car_assemble.h"

typedef struct Node {
    char string[50];
    PowertrainType trainType;
    ChassisRobotType chassisType;
    struct Node* next;
} Node;

typedef struct Queue {
    Node* head;
    Node* tail;
    int length;
} Queue;

void enqueue(Queue* queue, Node* node);
Node* dequeue(Queue* queue);
Node* newNode(char *str, PowertrainType trainType, ChassisRobotType chassisType);
Queue* newQueue();