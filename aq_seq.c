/**
 * @file   aq.c
 * @Author 02335 team
 * @date   October, 2024
 * @brief  Alarm queue skeleton implementation
 */

#include "aq.h"
#include "stdlib.h"

AlarmQueue aq_create( ) {
    Queue *queue = malloc(sizeof(Queue));
    if(queue == NULL){
        // Return NULL if the memory allocation fails
        return NULL;
    }

    // Initialize the queue
    queue->head = NULL;
    queue->tail = NULL;
    queue->alarm_msg = NULL;

    // Return the opaque pointer to the queue
    return (AlarmQueue)queue;
}

int aq_send( AlarmQueue aq, void * msg, MsgKind k){
    Queue  *queue = (Queue *)aq;

    // Check whether this message is an alarm
    if(k == AQ_ALARM) {
        if(queue->alarm_msg != NULL){
            // Alarm Message is already present, return an error.
            return AQ_NO_ROOM;
        }
        queue->alarm_msg = msg;

        // Return success
        return 0;
    }
    else {
        Node *newNode = malloc(sizeof(Node));

        if(newNode == NULL){
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

        // Return success
        return 0;
    }
}

int aq_recv( AlarmQueue aq, void **msg) {
    Queue *queue = (Queue *)aq;

    // Check whether this is an alarm message, a normal message or no message
    if(queue->alarm_msg != NULL) {
        // Set the pointer to the alarm message
        *msg = queue->alarm_msg;

        // Clear the alarm message from the queue
        queue->alarm_msg = NULL;
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
        return  AQ_NORMAL;
    }
    else {
        // If no messages are present in the queue.
        // Set the pointer to null
        *msg = NULL;
        return AQ_NO_MSG;
    }
}

int aq_size( AlarmQueue aq) {
    return 0;
}

int aq_alarms( AlarmQueue aq) {
    return 0;
}



