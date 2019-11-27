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

NODEptr who[NODE_SIZE] = {}, what[NODE_SIZE]= {}, where[NODE_SIZE] = {}, head, temp;

int knowledge_get(int inc, const char *intent, char *entity[], char *response, int n) {
	//printf("running knowledge_get, intent is: %s\n", intent);

	char strEntity[MAX_ENTITY] = "";
	for (int i = 0; i < inc; ++i) {
		strcat(strEntity,entity[i]);
		if (i<inc-1) {
			strcat(strEntity," ");
		}
	}

	//if user ask for date/time
	if (stristr(strEntity,"time")  != NULL || stristr(strEntity,"date")  != NULL) {
		time_t t = time(NULL);
		struct tm tm = *localtime(&t);
		if (stristr(strEntity,"time")  != NULL) {
			snprintf(response, n, "The time now is %02d:%02d",tm.tm_hour, tm.tm_min);
		}
		else {
			snprintf(response, n, "The date is %d-%02d-%02d", tm.tm_year + 1900, tm.tm_mon + 1,tm.tm_mday);
		}
		return KB_OK;
	}
	
	//if user ask for chatbot's name
	if (stristr(strEntity,"you")  != NULL || stristr(strEntity,"your name")  != NULL) {
		if (compare_token(intent, "where") == 0) {
			snprintf(response, n, "I over am here.");
		}
		else {
			snprintf(response, n, "I am %s", chatbot_botname());
		}
		
		return KB_OK;
	}
	
	//if answer is found in the linkedlist then return KB_OK (0)
	int intentflag;
	int index[inc];
	char* latest = NULL;

	for (int i = 0; i < inc; ++i) {
		if (!isalpha(entity[i][0])) {
			index[i] = NODE_SIZE - 1;
		}
		else {
			index[i] = tolower(entity[i][0]) % 'a'; //to find the first letter of entity in the head pointer array
		}
		
		//printf("index[i] is: %d\n", index[i]);
		if (compare_token(intent, "who") == 0){
			intentflag = 0;
			head = who[index[i]];
			//printf("Using who array\n");
		}
		else if (compare_token(intent, "what") == 0){
			intentflag = 1;
			head = what[index[i]];
		}
		else if (compare_token(intent, "where") == 0){
			intentflag = 2;
			head = where[index[i]];
		}
		else
			return KB_INVALID; // When intent is neither "who", "what" or "where"
		
		//checking if the entity is inside one of the linkedlist
		//printf("checking for the loops\n");
		if(head != NULL){
			temp = head;
			while(temp){
				//printf("checking temp entity: %s\n", temp->entity);
				//printf("checking temp entity: %s\n", entity[i]);
				if (compare_token(strEntity,temp->entity) == 0) {
					snprintf(response, n, temp->answer);
					return KB_OK;
				}
				else if (stristr(entity[i],temp->entity) != 0 || stristr(temp->entity,entity[i]) != 0){
					latest = temp->answer;
				}
				temp = temp->next;
			}

			if (latest != NULL) {
				snprintf(response, n, latest);
				return KB_OK;
			}
			
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

	int index;
	if (!isalpha(entity[0])) {
		index = NODE_SIZE - 1;
	}
	else {
		//printf("%d, %d", tolower(entity[0]), 'a');
		index = tolower(entity[0]) % 'a'; //to find the first letter of entity in the head pointer array
	}

	char test[2] = "";
	//printf("running knowledge_put\n");
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
	
	if (compare_token(response, test) == 0) // if userinput is just enter then it will exit knowledge_put
		return KB_INVALID;
	
	NODEptr newknowledge = (NODEptr)malloc(sizeof(NODE));
	newknowledge->answer = (char *)malloc(sizeof(char)*strlen(response)+1);
	newknowledge->entity = (char *)malloc(sizeof(char)*strlen(entity)+1);
	if (newknowledge == NULL)//  || newknowledge->response == NULL || newknowledge->entity == NULL
		return KB_NOMEM; // return -3 when cannot allocate any memory for node
	
	// populate the newknowledge
	strcpy(newknowledge->entity, entity);
	strcpy(newknowledge->answer, response);
	newknowledge->next = NULL;
	
	//printf("What is inside new node: %s %s\n", newknowledge->entity, newknowledge->answer);
	
	//do the checking
	if (head != NULL){ // check if the head is empty
		if (head->next != NULL){ // check if the node after head is empty
			temp = head->next;
			while(temp != NULL){ // To check if there is any node in the linkedlist having the same entity as new node
				if (compare_token(temp->entity, newknowledge->entity) == 0){ // old node has the same entity as the new node (applies to 2nd node only)
					free(temp->entity);
					free(temp->answer);
					head->next = newknowledge;
					return KB_OK;
				}
				else if (temp->next == NULL){ // when the node next to temp is empty
					temp->next = newknowledge;
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
				//printf("Added to who array\n");
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
			//printf("What is inside head node: %s %s\n", head->entity, head->answer);
			if (compare_token(head->entity, newknowledge->entity) == 0){ // when head entity same as new node entity 
				//printf("New node same as old node!\n");
				free(head->entity);
				free(head->answer);
				free(head);
				head = newknowledge;
				if(intentflag == 0){
					who[index] = head;
					//printf("Added to who array\n");
				}
				else if (intentflag == 1){
					what[index] = head;
				}
				else if (intentflag == 2){
					where[index] = head;
				}
				return KB_OK;
				
			}
			else { // when head entity is different from new node entity
				//printf("New node different from old node!\n");
				head->next = newknowledge;
				return KB_OK;
			}
		}
	}
	else{
		//printf("There is no head!\n");
		head = newknowledge;
		if(intentflag == 0){
			who[index] = head;
			//printf("Added to who array\n");
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
int knowledge_read(LPCSTR ini) {
	int count = 0;
	count += read_section("what", ini);
	count += read_section("where", ini);
	count += read_section("who", ini);
	return count;
}

int read_section (char *section, LPCSTR ini) {
	int count = 0;

	// Get all keys in the section
	char ini_keys[MAX_KEYS];
	GetPrivateProfileString(section, NULL, NULL, ini_keys, MAX_KEYS, ini);

	char *ini_key = ini_keys;

	// Loop through all entities in current section
	while (*ini_key) {
		NODEptr* listPtr = NULL;
		char temp_response[MAX_RESPONSE];
		GetPrivateProfileString(section, ini_key, NULL, temp_response, MAX_RESPONSE, ini);
		int result = 0;

		//printf("%d\n%s\n%s\n\n", index, temp->entity, temp->answer);

		if (section == "what") {
			result = knowledge_put("what", ini_key, temp_response);
		}
		else if (section == "who"){ 
			result = knowledge_put("who", ini_key, temp_response);
		}else{
			result = knowledge_put("where", ini_key, temp_response);
		}
		if (result == 0) {
			count++;
			ini_key += strlen(ini_key) + 1;
		}
		else{
			//printf("Error in file reading!");
			return -1;
		}

		//printf("Current result is: %d\n", result);
	}
	return count;
}


/*
 * Reset the knowledge base, removing all know entitities from all intents.
 */
void knowledge_reset() {
	
	/* to be implemented */
	for (int i = 0; i<NODE_SIZE; i++){ // looping through the array of head pointers
		if(who[i] != NULL){
			head = who[i];
			while(head){
				temp = head;
				head = head->next;
				free(temp->entity);
				free(temp->answer);
				free(temp);
			}
		}
		if(what[i] != NULL){
			head = what[i];
			while(head){
				temp = head;
				head = head->next;
				free(temp->entity);
				free(temp->answer);
				free(temp);
			}
		}
		if(where[i] != NULL){
			head = where[i];
			while(head){
				temp = head;
				head = head->next;
				free(temp->entity);
				free(temp->answer);
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
void knowledge_write(LPCSTR ini) {
	write_section("what", ini);
	write_section("who", ini);
	write_section("where", ini);
}

void write_section (char *section, LPCSTR ini) {
	for (int i = 0; i < NODE_SIZE; ++i) {
		if (section == "what") {
			head = what[i];
		}
		else if (section == "who") {
			head = who[i];
		}
		else {
			head = where[i];
		}
		while (head != NULL) {
			WritePrivateProfileStringA(section, head->entity, head->answer, ini);
			head = head->next;
		}
	}
}

void toLowerStr(char *str)
{
    for (int i = 0; i < strlen(str); ++i)
        str[i] = tolower(str[i]);
}

char* stristr( const char* str1, const char* str2 )
{
    const char* p1 = str1 ;
    const char* p2 = str2 ;
    const char* r = *p2 == 0 ? str1 : 0 ;

    while( *p1 != 0 && *p2 != 0 )
    {
        if( tolower( (unsigned char)*p1 ) == tolower( (unsigned char)*p2 ) )
        {
            if( r == 0 )
            {
                r = p1 ;
            }

            p2++ ;
        }
        else
        {
            p2 = str2 ;
            if( r != 0 )
            {
                p1 = r + 1 ;
            }

            if( tolower( (unsigned char)*p1 ) == tolower( (unsigned char)*p2 ) )
            {
                r = p1 ;
                p2++ ;
            }
            else
            {
                r = 0 ;
            }
        }

        p1++ ;
    }

    return *p2 == 0 ? (char*)r : 0 ;
}