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

NODEptr who[NODE_SIZE] = {};
NODEptr what[NODE_SIZE]= {};
NODEptr where[NODE_SIZE] = {}; 
NODEptr head, temp;

int knowledge_get(int inc, const char *intent, char *entity[], char *response, int n) {

	// Store the entity in a string
	char strEntity[MAX_ENTITY] = "";
	for (int i = 0; i < inc; ++i) {
		strcat(strEntity,entity[i]);
		if (i<inc-1) {
			strcat(strEntity," ");
		}
	}

	// Respond to date/time query
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
	
	// Respond to name query
	if (stristr(strEntity,"you")  != NULL || stristr(strEntity,"your name")  != NULL) {
		if (compare_token(intent, "where") == 0) {
			snprintf(response, n, "I over am here.");
		}
		else {
			snprintf(response, n, "I am %s", chatbot_botname());
		}
		
		return KB_OK;
	}

	int index[inc];
	char* latest = NULL;

	for (int i = 0; i < inc; ++i) {
		if (!isalpha(entity[i][0])) {
			index[i] = NODE_SIZE - 1;
		}
		else {
			index[i] = tolower(entity[i][0]) % 'a'; //to find the first letter of entity in the head pointer array
		}
		
		if (compare_token(intent, "who") == 0){
			head = who[index[i]];
		}
		else if (compare_token(intent, "what") == 0){
			head = what[index[i]];
		}
		else if (compare_token(intent, "where") == 0){
			head = where[index[i]];
		}
		else
			return KB_INVALID; // When intent is neither "who", "what" or "where"
		
		//checking if the entity is inside one of the linkedlist
		if(head != NULL){
			temp = head;
			while(temp){
				// Check for exact match
				if (compare_token(strEntity,temp->entity) == 0) {
					snprintf(response, n, temp->answer);
					return KB_OK;
				}
				// Fuzzy search
				else if (stristr(entity[i],temp->entity) != 0 || stristr(temp->entity,entity[i]) != 0){
					latest = temp->answer;
				}
				temp = temp->next;
			}

			// return KB_FUZZY if search is found, but isn't exact
			if (latest != NULL) {
				printf("%s: %s\n",chatbot_botname(), latest);
				return KB_FUZZY;
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
 *   KB_FUZZY, if non-exact match is found
 */
int knowledge_put(const char *intent, const char *entity, const char *response) {

	int intentflag;

	int index;
	if (!isalpha(entity[0])) {
		index = NODE_SIZE - 1;
	}
	else {
		index = tolower(entity[0]) % 'a'; //to find the first letter of entity in the head pointer array
	}

	char test[2] = "";
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
	if (newknowledge == NULL)
		return KB_NOMEM; // return -3 when cannot allocate any memory for node
	
	// populate the newknowledge
	strcpy(newknowledge->entity, entity);
	strcpy(newknowledge->answer, response);
	newknowledge->next = NULL;
	
	//do the checking
	if (head != NULL){ // check if the head is empty
		temp = head;
		if(compare_token(temp->entity, newknowledge->entity) == 0) { // check if new node same as head node
			newknowledge->next = temp->next;
			free(temp->entity);
			free(temp->answer);
			free(temp);
			head = newknowledge;
			if(intentflag == 0){
				who[index] = head;
			}
			else if (intentflag == 1){
				what[index] = head;
			}
			else if (intentflag == 2){
				where[index] = head;
			}
			return KB_OK;
		}
		else{
			if(temp->next == NULL){ // check if next node is empty
				temp->next = newknowledge;
				return KB_OK;
			}
			else{
				while(temp->next != NULL){ // keep checking for next node
					if(compare_token(temp->next->entity, newknowledge->entity) == 0){ //if they are the same as new node then will replace
						temp->next = newknowledge;
						free(temp->next->entity);
						free(temp->next->answer);
						free(temp->next);
						return KB_OK;
					}
					temp = temp->next;
				} //if there are no nodes same as new node, then it will just place new node at the end
				temp->next = newknowledge;
				return KB_OK;
			}
			
		}
	}
	else{
		head = newknowledge;
		if(intentflag == 0){
			who[index] = head;
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

/*
 * Read data from ini file and store into knowledge base
 * 
 * Input:
 *   section - the section to be read
 *   ini - location of the ini file
 */
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

		// Store data into knowledge base
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
			return -1;
		}
	}
	return count;
}


/*
 * Reset the knowledge base, removing all know entitities from all intents.
 */
void knowledge_reset() {
	
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

/*
 * Save knowledge base to ini file
 * 
 * Input:
 *   section - the section to be written to
 *   ini - location of the ini file
 */
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

/*
 * String implementation of tolower() function
 * 
 * Input:
 *   str - pointer to the string
 */
void toLowerStr(char *str)
{
    for (int i = 0; i < strlen(str); ++i)
        str[i] = tolower(str[i]);
}

/*
 * Case in-sensitive implementation of strstr()
 * 
 * Input:
 *   str1 - pointer to the string
 *   str2 - pointer to the substring
 * Returns: pointer to the location of substring in string
 */
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