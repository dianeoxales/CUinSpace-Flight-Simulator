#include "defs.h"
#include <stdlib.h>
#include <stdio.h>

/* Event functions */

/**
 * Initializes an `Event` structure.
 *
 * Sets up an `Event` with the provided system, resource, status, priority, and amount.
 *
 * @param[out] event     Pointer to the `Event` to initialize.
 * @param[in]  system    Pointer to the `System` that generated the event.
 * @param[in]  resource  Pointer to the `Resource` associated with the event.
 * @param[in]  status    Status code representing the event type.
 * @param[in]  priority  Priority level of the event.
 * @param[in]  amount    Amount related to the event (e.g., resource amount).
 */
void event_init(Event *event, System *system, Resource *resource, int status, int priority, int amount) {
    event->system = system;
    event->resource = resource;
    event->status = status;
    event->priority = priority;
    event->amount = amount;
}

/* EventQueue functions */

/**
 * Initializes the `EventQueue`.
 *
 * Sets up the queue for use, initializing any necessary data (e.g., semaphores when threading).
 *
 * @param[out] queue  Pointer to the `EventQueue` to initialize.
 */
void event_queue_init(EventQueue *queue) {
    queue->head = NULL;
    queue->size = 0;
    if (sem_init(&queue->mutex, 0, 1) != 0) {
        perror("Error: on sem init for event queue.\n");
        exit(EXIT_FAILURE);
    }
}

/**
 * Cleans up the `EventQueue`.
 *
 * Frees any memory and resources associated with the `EventQueue`.
 * 
 * @param[in,out] queue  Pointer to the `EventQueue` to clean.
 */
void event_queue_clean(EventQueue *queue) {
    EventNode *current = queue->head;
    EventNode *next;
    // traverse through the linked list and free each node
    while (current != NULL) {
        next = current->next;
        free(current);
        current = next;
    }

    queue->head = NULL;
    queue->size = 0; 
    sem_destroy(&queue->mutex);
}

/**
 * Pushes an `Event` onto the `EventQueue`.
 *
 * Adds the event to the queue in a thread-safe manner, maintaining priority order (highest first).
 *
 * @param[in,out] queue  Pointer to the `EventQueue`.
 * @param[in]     event  Pointer to the `Event` to push onto the queue.
 */
void event_queue_push(EventQueue *queue, const Event *event) {
    if (!queue || !event) {
        return;
    }
    sem_wait(&queue->mutex); // lock before adding

    // create a new node to store the event
    EventNode *new_node = (EventNode *) malloc(sizeof(EventNode));
    if (new_node == NULL) {
        return;
    }

    new_node->event = *event;
    new_node->next = NULL;

    // if the queue is empty, insert the new node at the head
    if (queue->head == NULL) {
        queue->head = new_node;
        queue->size = 1;
        return;
    }

    EventNode *current = queue->head;
    EventNode *previous = NULL;

    // find the correct insertion position
    while(current != NULL && current->event.priority >= event->priority) {
        if (current->event.priority == event->priority) { // make sure the event->priority is before the new one 
            break;
        }

        previous = current;
        current = current->next;
    }

    // insert the new node in the correct position 
    if (previous == NULL) {
        new_node->next = queue->head;
        queue->head = new_node;
    } else {
        // insert after previous node
        previous->next = new_node;
        new_node->next = current;
    }

    queue->size++; // increment the size of the queue
    sem_post(&queue->mutex); // unlock after
}

/**
 * Pops an `Event` from ===]== `EventQueue`.
 *
 * Removes the highest priority event from the queue in a thread-safe manner.
 *
 * @param[in,out] queue  Pointer to the `EventQueue`.
 * @param[out]    event  Pointer to the `Event` structure to store the popped event.
 * @return               Non-zero if an event was successfully popped; zero otherwise.
 */
int event_queue_pop(EventQueue *queue, Event *event) {
    if(!queue || !event || queue->head == NULL) {
        return 0;
    }
    // sem_wait(&queue->mutex);
    if (queue != NULL && queue->head != NULL) {
        EventNode *node_pop = queue->head; // get the node at the head of the queue
        *event = node_pop->event; // copy the event data from the popped node to the event argument 
        queue->head = node_pop->next;
        free(node_pop);
        queue->size--; // decrement size of the queue
        sem_post(&queue->mutex);
        return 1;
    }
    sem_post(&queue->mutex);
    return 0;
}