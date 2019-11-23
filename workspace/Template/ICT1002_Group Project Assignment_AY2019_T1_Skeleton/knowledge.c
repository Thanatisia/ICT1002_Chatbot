/*
 * ICT1002 (C Language) Group Project, AY19 Trimester 1.
 *
 * This file implements the chatbot's knowledge base.
 *
 * knowledge_get() retrieves the response to a question.
 * knowledge_put() inserts a new response to a question.
 * knowledge_read() reads the knowledge base from a file.
 * knowledge_reset() erases all of the knowledge.
 * kowledge_write() saves the knowledge base in a file.
 *
 * You may add helper functions as necessary.
 */
 
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "chat1002.h"

/*
 * Get the response to a question.
 *
 * Input:
 *   intent   - the question word
 *   entity   - the entity
 *   response - a buffer to receive the response
 *   n        - the maximum number of characters to write to the response buffer
 *
 * Returns:
 *   KB_OK, if a response was found for the intent and entity (the response is copied to the response buffer)
 *   KB_NOTFOUND, if no response could be found
 *   KB_INVALID, if 'intent' is not a recognised question word
 */
 
typedef struct node {
	char *entity;
	char *answer;
	struct node *next;
}NODE, *NODEptr;

NODEptr who[26] = {}, what[26]= {}, where[26] = {}, head, temp; // 26 for 26 different alphabets

int knowledge_get(const char *intent, const char *entity, char *response, int n) {
	
	/* to be implemented */
	printf("running knowledge_get, intent is: %s\n", intent);
	printf("entity is: %s\n", entity);
	
	//if answer is found in the linkedlist then return KB_OK (0)
	int intentflag;
	int index = entity[0]%26; //to find the first letter of entity in the head pointer array
	printf("index is: %d\n", index);
	if (compare_token(intent, "who") == 0){
		intentflag = 0;
		head = who[index];
		printf("Using who array\n");
	}
	else if (compare_token(intent, "what") == 0){
		intentflag = 1;
		head = what[index];
	}
	else if (compare_token(intent, "where") == 0){
		intentflag = 2;
		head = where[index];
	}
	else
		return KB_INVALID; // When intent is neither "who", "what" or "where"
	
	//checking if the entity is inside one of the linkedlist
	printf("checking for the loops\n");
	
	if(head != NULL){
		temp = head;
		while(temp){
			printf("checking temp entity: %s\n", temp->entity);
			if (compare_token(temp->entity, entity) == 0){
				snprintf(response, n, temp->answer);
				return KB_OK;
			}
			temp = temp->next;
		}
	}
	
	
	//else answer not found for intent and entity pair then return KB_NOTFOUND (-1)
	
	return KB_NOTFOUND;
	
}


/*
 * Insert a new response to a question. If a response already exists for the
 * given intent and entity, it will be overwritten. Otherwise, it will be added
 * to the knowledge base.
 *
 * Input:
 *   intent    - the question word
 *   entity    - the entity
 *   response  - the response for this question and entity
 *
 * Returns:
 *   KB_FOUND, if successful
 *   KB_NOMEM, if there was a memory allocation failure
 *   KB_INVALID, if the intent is not a valid question word
 */
int knowledge_put(const char *intent, const char *entity, const char *response) {
	
	/* to be implemented */
	int intentflag;
	int index = entity[0]%26; //to find the first letter of entity in the head pointer array
	printf("running knowledge_put\n");
	if (compare_token(intent, "who") == 0){
		intentflag = 0;
		head = who[index];
	}
	else if (compare_token(intent, "what") == 0){
		intentflag = 1;
		head = what[index];
	}
	else if (compare_token(intent, "where") == 0){
		intentflag = 2;
		head = where[index];
	}
	else
		return KB_INVALID; // return -2 when intent is not either "who", "what" and "where"
	printf("What user typed: %s\n", response);
	
	NODEptr newknowledge = (NODEptr)malloc(sizeof(NODE));
	newknowledge->answer = (char *)malloc(sizeof(char)*strlen(response));
	newknowledge->entity = (char *)malloc(sizeof(char)*strlen(entity));
	if (newknowledge == NULL)//  || newknowledge->response == NULL || newknowledge->entity == NULL
		return KB_NOMEM; // return -3 when cannot allocate any memory for node
	
	// populate the newknowledge
	strcpy(newknowledge->entity, entity);
	strcpy(newknowledge->answer, response);
	
	printf("What is inside new node: %s %s\n", newknowledge->entity, newknowledge->answer);
	
	//do the checking
	if (head != NULL){ // check if the head is empty
		if (head->next != NULL){ // check if the node after head is empty
			temp = head->next;
			while(temp != NULL){ // To check if there is any node in the linkedlist having the same entity as new node
				if (compare_token(temp->entity, newknowledge->entity) == 0){ // old node has the same entity as the new node (applies to 2nd node only)
					free(temp->entity);
					free(temp->answer);
					newknowledge->next = NULL;
					head->next = newknowledge;
					return KB_OK;
				}
				else if (temp->next == NULL){ // when the node next to temp is empty
					temp->next = newknowledge;
					newknowledge->next = NULL;
					return KB_OK;
				}
				else if(compare_token(temp->next->entity, newknowledge->entity) == 0){ // when the next node of temp is the same as new node
					if(temp->next->next != NULL){
						newknowledge->next = temp->next->next;
						free(temp->next->entity);
						free(temp->next->answer);
						temp->next = newknowledge;
						return KB_OK;
					}
					else{ // When temp->next->next is NULL
						newknowledge->next = NULL;
						free(temp->next->entity);
						free(temp->next->answer);
						temp->next = newknowledge;
						return KB_OK;
					}
				}
				temp = temp->next;
			}
			//if there is no node in the linkedlist having the same entity as new node
			newknowledge->next = head;
			head = newknowledge;
			if(intentflag == 0){
				who[index] = head;
				printf("Added to who array\n");
			}
			else if (intentflag == 1){
				what[index] = head;
			}
			else if (intentflag == 2){
				where[index] = head;
			}
			return KB_OK;
		}
		else{ // if there is only 1 node inside linkedlist
			if (compare_token(head->entity, newknowledge->entity) != 0){ // when head entity is different from new node entity
				newknowledge->next = NULL;
				head->next = newknowledge;
				return KB_OK;
			}
			else { // when head entity same as new node entity
				free(head->entity);
				free(head->answer);
				head = newknowledge;
				if(intentflag == 0){
					who[index] = head;
					printf("Added to who array\n");
				}
				else if (intentflag == 1){
					what[index] = head;
				}
				else if (intentflag == 2){
					where[index] = head;
				}
				return KB_OK;
			}
		}
	}
	else{
		newknowledge->next = NULL;
		head = newknowledge;
		if(intentflag == 0){
			who[index] = head;
			printf("Added to who array\n");
		}
		else if (intentflag == 1){
			what[index] = head;
		}
		else if (intentflag == 2){
			where[index] = head;
		}
		return KB_OK;
	}
	
	
	
	return KB_INVALID;
}


/*
 * Read a knowledge base from a file.
 *
 * Input:
 *   f - the file
 *
 * Returns: the number of entity/response pairs successful read from the file
 */
int knowledge_read(FILE *f) {
	
	/* to be implemented */
	
	return 0;
}


/*
 * Reset the knowledge base, removing all know entitities from all intents.
 */
void knowledge_reset() {
	
	/* to be implemented */
	for (int i = 0; i<26; i++){ // looping through the array of head pointers
		if(who[i] != NULL){
			head = who[i];
			while(head){
				temp = head;
				head = head->next;
				free(temp->entity);
				free(temp->answer);
				free(temp->next);
				free(temp);
			}
		}
		if(what[i] != NULL){
			head = who[i];
			while(head){
				temp = head;
				head = head->next;
				free(temp->entity);
				free(temp->answer);
				free(temp->next);
				free(temp);
			}
		}
		if(where[i] != NULL){
			head = who[i];
			while(head){
				temp = head;
				head = head->next;
				free(temp->entity);
				free(temp->answer);
				free(temp->next);
				free(temp);
			}
		}
		who[i] = NULL;
		what[i] = NULL;
		where[i] = NULL;
	}
	
}


/*
 * Write the knowledge base to a file.
 *
 * Input:
 *   f - the file
 */
void knowledge_write(FILE *f) {
	
	/* to be implemented */
	
	
}