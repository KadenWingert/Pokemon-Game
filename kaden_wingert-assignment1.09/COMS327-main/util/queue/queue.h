#ifndef QUEUE_H
#define QUEUE_H

typedef struct QueueNode {
  void* data;
  struct QueueNode* next;
} QueueNode;

typedef struct Queue {
  QueueNode* head;
  QueueNode* tail;
  int size;
} Queue;

int createQueue(Queue* queue);
int enqueue(Queue* queue, void* data);
int dequeue(Queue* queue, void** data);
int destroyQueue(Queue* queue);

#endif