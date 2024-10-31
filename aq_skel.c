/**
 * @file   aq.c
 * @Author 02335 team
 * @date   October, 2024
 * @brief  Alarm queue skeleton implementation
 */

#include "aq.h"
#include "stdlib.h"

AlarmQueue aq_create( ) {
    Queue  *queue = malloc(sizeof(Queue));
    if(!queue) {
        // Return null if the memory allocation fails
        return NULL;
    }

    // Initialize the Queue
    queue->head = NULL;
    queue->tail = NULL;
    queue->alarm_msg = NULL;

    // Return the opaque pointer to the new Queue
    return (AlarmQueue)queue;
}

int aq_send( AlarmQueue aq, void * msg, MsgKind k){
  return AQ_NOT_IMPL;
}

int aq_recv( AlarmQueue aq, void * * msg) {
  return AQ_NOT_IMPL;
}

int aq_size( AlarmQueue aq) {
  return 0;
}

int aq_alarms( AlarmQueue aq) {
  return 0;
}



