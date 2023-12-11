#include "queue.h"

#include <stdlib.h>

int createQueue(Queue *queue) {
  queue->head = NULL;
  queue->tail = NULL;
  queue->size = 0;

  return 0;
}

int enqueue(Queue *queue, void *data) {
  QueueNode *node = malloc(sizeof(QueueNode));
  node->data = data;
  node->next = NULL;

  if (queue->size == 0) {
    queue->head = node;
    queue->tail = node;
  } else {
    queue->tail->next = node;
    queue->tail = node;
  }

  queue->size++;

  return 0;
}

int dequeue(Queue *queue, void **data) {
  if (queue->size == 0) {
    return 1;
  }

  *data = queue->head->data;

  QueueNode *node = queue->head;
  queue->head = queue->head->next;
  free(node);

  queue->size--;

  return 0;
}

int destroyQueue(Queue *queue) {
  while (queue->size > 0) {
    QueueNode *node = queue->head;
    queue->head = queue->head->next;
    free(node);

    queue->size--;
  }

  return 0;
}