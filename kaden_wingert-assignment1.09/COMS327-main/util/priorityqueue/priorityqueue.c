#include "priorityqueue.h"

#include <stdlib.h>

int createPriorityQueue(PriorityQueue *queue) {
  queue->head = NULL;
  queue->tail = NULL;
  queue->size = 0;

  return 0;
}

int enqueueWithPriority(PriorityQueue *queue, void *data, int priority) {
  PriorityQueueNode *node = malloc(sizeof(PriorityQueueNode));
  node->data = data;
  node->next = NULL;
  node->priority = priority;

  if (queue->size == 0) {
    queue->head = node;
    queue->tail = node;
  } else {
    if (priority < queue->head->priority) {
      node->next = queue->head;
      queue->head = node;
    } else {
      PriorityQueueNode *current = queue->head;
      while (current->next != NULL && current->next->priority <= priority) {
        current = current->next;
      }
      node->next = current->next;
      current->next = node;
    }
  }

  queue->size++;

  return 0;
}

int dequeueWithPriority(PriorityQueue *queue, void **data) {
  if (queue->size == 0) {
    return 1;
  }

  *data = queue->head->data;

  PriorityQueueNode *node = queue->head;
  queue->head = queue->head->next;
  free(node);

  queue->size--;

  return 0;
}

int destroyPriorityQueue(PriorityQueue *queue) {
  while (queue->size > 0) {
    PriorityQueueNode *node = queue->head;
    queue->head = queue->head->next;
    free(node);

    queue->size--;
  }

  return 0;
}