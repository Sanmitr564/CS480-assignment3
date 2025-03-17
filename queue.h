struct Node {
    char string[50];
    PowertrainType trainType;
    ChassisRobotType chassisType;
    Node* next;
}typedef Node;

struct Queue {
    Node* head;
    Node* tail;
    int length;
}typedef Queue;

int enqueue(Queue* queue, Node* node);
Node* dequeue(Queue* queue);
Node* newNode(char *str, PowertrainType trainType, ChassisRobotType chassisType);
Queue* newQueue();