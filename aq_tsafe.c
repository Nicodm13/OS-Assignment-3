/**
 * @file   aq.c
 * @Author 02335 team
 * @date   October, 2024
 * @brief  Alarm queue skeleton implementation
 */

#include "aq.h"
#include "stdlib.h"

typedef struct Node {
    void *message;
    struct Node *next;
} Node;

typedef struct {
    Node *head;
    Node *tail;
    void *alarm_msg;
    pthread_mutex_t lock;
    pthread_cond_t msg_cond;
    pthread_cond_t alarm_cond;
} ThreadSafeQueue;

AlarmQueue aq_create( ) {
    ThreadSafeQueue *queue = malloc(sizeof(ThreadSafeQueue));
    if(queue == NULL){
        // Return NULL if the memory allocation fails
        return NULL;
    }

    // Initialize the queue
    queue->head = NULL;
    queue->tail = NULL;
    queue->alarm_msg = NULL;
    pthread_mutex_init(&queue->lock, NULL);
    pthread_cond_init(&queue->msg_cond, NULL);
    pthread_cond_init(&queue->alarm_cond, NULL);

    // Return the opaque pointer to the queue
    return (AlarmQueue)queue;
}

int aq_send(AlarmQueue aq, void *msg, MsgKind k) {
    ThreadSafeQueue *queue = (ThreadSafeQueue *)aq;
    pthread_mutex_lock(&queue->lock);

    if (k == AQ_ALARM) {
        while (queue->alarm_msg != NULL) {
            // Block until the alarm message is consumed
            pthread_cond_wait(&queue->alarm_cond, &queue->lock);
        }

        queue->alarm_msg = msg;
        pthread_cond_signal(&queue->msg_cond);
        pthread_mutex_unlock(&queue->lock);
        return 0;
    }

    Node *newNode = malloc(sizeof(Node));
    if (newNode == NULL) {
        pthread_mutex_unlock(&queue->lock);
        return AQ_UNINIT;
    }

    newNode->message = msg;
    newNode->next = NULL;

    if (queue->tail == NULL) {
        queue->head = newNode;
        queue->tail = newNode;
    } else {
        queue->tail->next = newNode;
        queue->tail = newNode;
    }

    pthread_cond_signal(&queue->msg_cond);
    pthread_mutex_unlock(&queue->lock);
    return 0;
}



int aq_recv( AlarmQueue aq, void **msg) {
    ThreadSafeQueue *queue = (ThreadSafeQueue *)aq;
    pthread_mutex_lock(&queue->lock);

    // Wait until a message is available
    while (queue->head == NULL && queue->alarm_msg == NULL) {
        pthread_cond_wait(&queue->msg_cond, &queue->lock);
    }

    // Prioritize alarm messages
    if (queue->alarm_msg != NULL) {
        *msg = queue->alarm_msg;
        queue->alarm_msg = NULL;
        pthread_cond_signal(&queue->alarm_cond);
        pthread_mutex_unlock(&queue->lock);
        return AQ_ALARM;
    }

    // Handle normal messages
    Node *temp = queue->head;
    *msg = temp->message;
    queue->head = temp->next;
    if (queue->head == NULL) {
        queue->tail = NULL;
    }
    free(temp);

    pthread_mutex_unlock(&queue->lock);
    return AQ_NORMAL;
}

int aq_size( AlarmQueue aq) {
    ThreadSafeQueue *queue = (ThreadSafeQueue * )aq;
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
    ThreadSafeQueue *queue = (ThreadSafeQueue * )aq;

    // Return 1 to if there is an alarm message present, otherwise return 0
    return (queue->alarm_msg != NULL ? 1 : 0);
}



