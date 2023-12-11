#ifndef PRIORITY_QUEUE_H
#define PRIORITY_QUEUE_H

typedef struct PriorityQueueNode {
  void* data;
  int priority;
  struct PriorityQueueNode* next;
} PriorityQueueNode;

typedef struct PriorityQueue {
  PriorityQueueNode* head;
  PriorityQueueNode* tail;
  int size;
} PriorityQueue;

int createPriorityQueue(PriorityQueue* queue);
int enqueueWithPriority(PriorityQueue* queue, void* data, int priority);
int dequeueWithPriority(PriorityQueue* queue, void** data);
int destroyPriorityQueue(PriorityQueue* queue);

#endif