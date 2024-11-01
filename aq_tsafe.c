/**
 * @file   aq.c
 * @Author 02335 team
 * @date   October, 2024
 * @brief  Alarm queue skeleton implementation
 */

#include "aq.h"
#include "stdlib.h"

AlarmQueue aq_create( ) {
    SafeThreadQueue *queue = malloc(sizeof(SafeThreadQueue));
    if(queue == NULL){
        // Return NULL if the memory allocation fails
        return NULL;
    }

    // Initialize the queue
    queue->head = NULL;
    queue->tail = NULL;
    queue->alarm_msg = NULL;

    // Initialize the thread safety for the queue
    pthread_mutex_init(&queue->lock, NULL);
    pthread_cond_init(&queue->msg_cond, NULL);
    pthread_cond_init(&queue->alarm_cond, NULL);

    // Return the opaque pointer to the queue
    return (AlarmQueue)queue;
}

int aq_send( AlarmQueue aq, void * msg, MsgKind k){
    SafeThreadQueue *queue = (SafeThreadQueue *)aq;

    // Lock the queue for any other threads
    pthread_mutex_lock(&queue->lock);

    // Check whether this message is an alarm
    if(k == AQ_ALARM) {
        while (queue->alarm_msg != NULL){
            // Alarm Message is already present, block all threads until the alarm has been processed
            pthread_cond_wait(&queue->alarm_cond, &queue->lock);

            // Return an error
            return AQ_NO_ROOM;
        }
        // Set the new alarm message
        queue->alarm_msg = msg;

        // Signal that an alarm has been added
        pthread_cond_signal(&queue->alarm_cond);

        // Return success and unlock the queue
        pthread_mutex_unlock(&queue->lock);
        return 0;
    }
    else {
        Node *newNode = malloc(sizeof(Node));

        if(newNode == NULL){
            // Unlock the queue, as no node has been created
            pthread_mutex_unlock(&queue->lock);

            // Return uninitialized as the memory allocation failed
            return AQ_UNINIT;
        }

        // Initialize the new node, which is last in the list
        newNode->message = msg;
        newNode->next = NULL;

        // Check if the queue is empty
        if(queue->tail == NULL) {
            // Queue is empty, the new node is both head and tail
            queue->head = newNode;
            queue->tail = newNode;
        }
        else{
            // If the queue is not empty, append the new node at the end
            queue->tail->next = newNode;
            queue->tail = newNode;
        }

        // Signal that a normal message has been added
        pthread_cond_signal(&queue->msg_cond);

        // Return success and unlock the queue
        pthread_mutex_unlock(&queue->lock);
        return 0;
    }
}


int aq_recv( AlarmQueue aq, void **msg) {
    SafeThreadQueue *queue = (SafeThreadQueue *)aq;
    pthread_mutex_lock(&queue->lock);

    // Wait until a message is available
    while (queue->head == NULL && queue->alarm_msg == NULL) {
        pthread_cond_wait(&queue->msg_cond, &queue->lock);
    }

    // Check whether this is an alarm message, a normal message or no message
    if(queue->alarm_msg != NULL) {
        // Set the pointer to the alarm message
        *msg = queue->alarm_msg;

        // Clear the alarm message from the queue
        queue->alarm_msg = NULL;

        // Signal an alarm message for the queue
        pthread_cond_signal(&queue->alarm_cond);

        // Return a status message and unlock the mutex
        pthread_mutex_unlock(&queue->lock);
        return AQ_ALARM;
    }
    else if (queue->head != NULL) {
        // Set the pointer to the message of the head note
        *msg = queue->head->message;

        // Temporarily store the head note
        Node *temp = queue->head;

        // Move the head pointer to the next node
        queue->head = queue->head->next;

        // If the list is empty, also reset the tail
        if(queue->head == NULL){
            queue->tail = NULL;
        }

        // Free the memory of the original head note
        free(temp);

        // Return a status message and unlock the mutex
        pthread_mutex_unlock(&queue->lock);
        return  AQ_NORMAL;
    }
    else {
        // If no messages are present in the queue.
        // Set the pointer to null
        *msg = NULL;

        // Return a status message and unlock the mutex
        pthread_mutex_unlock(&queue->lock);
        return AQ_NO_MSG;
    }
}

int aq_size( AlarmQueue aq) {
    Queue *queue = (Queue * )aq;
    int count = 0;

    // Loop through the list to count the normal messages
    Node *current = queue->head;
    while (current != NULL){
        count++;
        current = current->next;
    }

    // Add 1 to count if there is an alarm message present
    return count + (queue->alarm_msg != NULL ? 1 : 0);
}

int aq_alarms( AlarmQueue aq) {
    Queue *queue = (Queue * )aq;

    // Return 1 to if there is an alarm message present, otherwise return 0
    return (queue->alarm_msg != NULL ? 1 : 0);
}



