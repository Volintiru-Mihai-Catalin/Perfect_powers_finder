#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct node {
    char* name;
    struct node *next;
}node, *Node;

typedef struct nodei {
    long var;
    struct nodei *next;
}nodei, *Nodei;

typedef struct list {
    Nodei head;
}l, *L;

typedef struct queue {
    Node head;
    Node tail;
}q, *Q;

void init_queue(Q queue) {
    queue->head = NULL;
    queue->tail = NULL;
}

L init_list() {
    L lis = (L)malloc(sizeof(l));
    lis->head = NULL;

    return lis;
}

void append(L lis, long var) {
    Nodei new_node = (Nodei)malloc(sizeof(nodei));
    new_node->next = NULL;
    new_node->var = var;

    if (lis->head == NULL) {
        lis->head = new_node;
        return;
    }

    Nodei tmp = lis->head;
    while (tmp->next != NULL) {
        tmp = tmp->next;
    }
    tmp->next = new_node;
    return;
}

void enqueue(Q queue, char* name) {

    Node newnode = (Node)malloc(sizeof(node));
    newnode->name = strdup(name);
    newnode->next = NULL;

    if (queue->tail != NULL) {
        queue->tail->next = newnode;
    }
    queue->tail = newnode;

    if (queue->head == NULL) {
        queue->head = newnode;
    }
    
}

char* dequeue(Q queue) {
    if (queue->head == NULL) {
        return NULL;
    }

    node *tmp = queue->head;
    char* name = tmp->name;
    queue->head = queue->head->next;
    if (queue->head == NULL) {
        queue->tail = NULL;
    }

    free(tmp);

    return name;
}

int is_empty(Q queue) {
    if (queue->head == NULL && queue->tail == NULL) {
        return 1;
    }
    return 0;
}

void destroy_list(L lis) {
    if (lis->head != NULL) {
        Nodei head = lis->head, tmp;
        while(head != NULL) {
            tmp = head;
            head = head->next;
            free(tmp);
        }
    }

    free(lis);
}

void destroy_queue(Q queue) {
    free(queue);
    return;
}

void print_list(L lis) {
    Nodei tmp = lis->head;
    while (tmp != NULL) {
        printf("%ld ", tmp->var);
        tmp = tmp->next;
    }
}

void sortedInsert(Nodei* head, Nodei newNode)
{
    nodei dummy;
    Nodei current = &dummy;
    dummy.next = *head;
 
    while (current->next != NULL && current->next->var < newNode->var) {
        current = current->next;
    }
 
    newNode->next = current->next;
    current->next = newNode;
    *head = dummy.next;
}

void insertSort(Nodei* head)
{
    Nodei result = NULL;    
    Nodei current = *head;   
    Nodei next;
 
    while (current != NULL)
    {
        next = current->next;
 
        sortedInsert(&result, current);
        current = next;
    }
 
    *head = result;
}

void mergeLists(L list1, L list2) {

    Nodei tmp = list2->head;
    while (tmp != NULL) {
        append(list1, tmp->var);
        tmp = tmp->next;
    }

    return;
}