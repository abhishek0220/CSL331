#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>

struct node {
   int data;
   struct node *next;
};

void print(struct node **head) {
    printf("Stack: "); 
    struct node *ptr = *head;
    while(ptr != NULL) {
        printf("%d ",ptr->data);
        ptr = ptr->next;
    }
    printf("\n");
}

void push(struct node **head, int data) {
   struct node *newNode = (struct node*) malloc(sizeof(struct node));
   newNode->data = data;
   newNode->next = *head;
   *head = newNode;
}

void delete(struct node **head) {
   struct node *temp = *head;
   *head = temp->next;
   free(temp);
}

void main() {
    struct node *stack1 = NULL;
    push(&stack1, 1);
    push(&stack1, 2);
    push(&stack1, 3);
    push(&stack1, 4);
    push(&stack1, 5);
    push(&stack1, 6);
    print(&stack1);
    delete(&stack1);
    delete(&stack1);
    print(&stack1);
    push(&stack1, 5);
    print(&stack1);
}