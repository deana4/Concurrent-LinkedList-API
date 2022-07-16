#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <stdbool.h>
#include "concurrent_list.h"

node* make_node(int value);

struct node {
	int value;
	pthread_mutex_t inner_mutex;
	struct node* next;
};

struct list {
	int element_num;
	struct node* head;
};

void print_node(node* node){
	if(node)
	{
		printf("%d ", node->value);
	}
}

list* create_list()
{
		struct list* new_list;
		new_list = (struct list*)malloc(sizeof(list));
		new_list -> element_num = 0;
		new_list -> head = NULL;
		return new_list;
}

void delete_list(list* list)
{
	if(list == NULL || list->head==NULL){
		return;
	}
	else {
		struct node* hold=list->head;
		struct node* remove;
		pthread_mutex_lock(&hold->inner_mutex);
		while((remove=hold->next)!=NULL){
			pthread_mutex_lock(&remove->inner_mutex); //check if ok
			struct node* temp=hold;
			pthread_mutex_unlock(&hold->inner_mutex);
			hold=remove;
			pthread_mutex_destroy(&temp->inner_mutex);
			free(temp);
		}
		pthread_mutex_unlock(&hold->inner_mutex);
		pthread_mutex_destroy(&hold->inner_mutex);
		free(hold);
		free(list);
	}
}

void insert_value(list* list, int value)
{
	if(list==NULL){
		return;
	}
	if(list->head==NULL){
		list->head=make_node(value);
		return;
	} else {
		pthread_mutex_lock(&list->head->inner_mutex);
		if(list->head->value >= value)
		{
			struct node* temp= list->head;
			list->head=make_node(value);
			list->head->next=temp;
			pthread_mutex_unlock(&temp->inner_mutex);
			return;
		} else {
			struct node* p=list->head;
			struct node* next;
			while((next=p->next)!=NULL) {
				pthread_mutex_lock(&next->inner_mutex);
				if(p->value<value && next->value>=value) {
					struct node* temp=p->next;
					p->next=make_node(value);
					p->next->next=temp;
					pthread_mutex_unlock(&next->inner_mutex);
					pthread_mutex_unlock(&p->inner_mutex);
					return;
				}
				pthread_mutex_unlock(&p->inner_mutex);
				p=next;
			}
			p->next=make_node(value);
			pthread_mutex_unlock(&p->inner_mutex);
		}
	}
}


void remove_value(list* list, int value)
{
	if(list==NULL || list->head==NULL) {
		return;
	}
	else {
		struct node* current=list->head;
		pthread_mutex_lock(&current->inner_mutex);

		if(current->value==value) {
			list->head=current->next;
			pthread_mutex_unlock(&current->inner_mutex);
			pthread_mutex_destroy(&current->inner_mutex);
			free(current);
			return;
		}
		else {
			struct node* next;
			while((next=current->next)!=NULL) {
				pthread_mutex_lock(&next->inner_mutex);
				if(next->value == value) {
					current->next=next->next;
					pthread_mutex_unlock(&next->inner_mutex);
					pthread_mutex_unlock(&current->inner_mutex);
					next->next=NULL;
					pthread_mutex_destroy(&next->inner_mutex);
					free(next);
					return;
				}
				pthread_mutex_unlock(&current->inner_mutex);
				current=next;
			}

			pthread_mutex_unlock(&current->inner_mutex);
			return;
		}
	}
}

void print_list(list* list)
{
	if (list == NULL || list->head==NULL) {
		printf("\n");
		return;
	}
	else {
		struct node* holder = list->head;
		struct node* next;
		pthread_mutex_lock(&holder->inner_mutex);
		while ((next=holder->next)!=NULL) {
			pthread_mutex_lock(&next->inner_mutex);
			print_node(holder);
			pthread_mutex_unlock(&holder->inner_mutex);
			holder = next;
		}
		print_node(holder);
		pthread_mutex_unlock(&holder->inner_mutex);
		printf("\n");
		return;
	}
}

void count_list(list* list, int (*predicate)(int))
{
	int var;
	int count = 0; // DO NOT DELETE
	if(list==NULL || list->head==NULL)
		return;
	struct node* holder = list->head;
	struct node* next;
	pthread_mutex_lock(&holder->inner_mutex);
	while((next=holder->next)!=NULL){
		pthread_mutex_lock(&next->inner_mutex);
		var = (*predicate)(holder->value);
		if (var) {
			count++;
		}
		pthread_mutex_unlock(&holder->inner_mutex);
		holder = next;
	}
	var = (*predicate)(holder->value);
	if (var) {
		count++;
	}
	pthread_mutex_unlock(&holder->inner_mutex);
	printf("%d items were counted\n", count); // DO NOT DELETE
	return;
}

node* make_node(int value){
	struct node* new_node;
	new_node = (struct node*) malloc(sizeof(node));
	pthread_mutex_init (&new_node->inner_mutex, NULL); //NULL=defult init
	pthread_mutex_lock(&new_node->inner_mutex);
	new_node->next = NULL;
	new_node->value = value;
	pthread_mutex_unlock(&new_node->inner_mutex);
	return new_node;
}
