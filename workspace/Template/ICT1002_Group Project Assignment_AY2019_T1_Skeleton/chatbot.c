/*
 * ICT1002 (C Language) Group Project, AY19 Trimester 1.
 *
 * This file implements the behaviour of the chatbot. The main entry point to
 * this module is the chatbot_main() function, which identifies the intent
 * using the chatbot_is_*() functions then invokes the matching chatbot_do_*()
 * function to carry out the intent.
 *
 * chatbot_main() and chatbot_do_*() have the same method signature, which
 * works as described here.
 *
 * Input parameters:
 *   inc      - the number of words in the question
 *   inv      - an array of pointers to each word in the question
 *   response - a buffer to receive the response
 *   n        - the size of the response buffer
 *
 * The first word indicates the intent. If the intent is not recognised, the
 * chatbot should respond with "I do not understand [intent]." or similar, and
 * ignore the rest of the input.
 *
 * If the second word may be a part of speech that makes sense for the intent.
 *    - for WHAT, WHERE and WHO, it may be "is" or "are".
 *    - for SAVE, it may be "as" or "to".
 *    - for LOAD, it may be "from".
 * The word is otherwise ignored and may be omitted.
 *
 * The remainder of the input (including the second word, if it is not one of the
 * above) is the entity.
 *
 * The chatbot's answer should be stored in the output buffer, and be no longer
 * than n characters long (you can use snprintf() to do this). The contents of
 * this buffer will be printed by the main loop.
 *
 * The behaviour of the other functions is described individually in a comment
 * immediately before the function declaration.
 *
 * You can rename the chatbot and the user by changing chatbot_botname() and
 * chatbot_username(), respectively. The main loop will print the strings
 * returned by these functions at the start of each line.
 */
 
#include "chat1002.h"

// Array of words to be ignored when parsing question
char *ignoreWords[] = {"\0", "is", "are", "the", "it", "at", "on"};

/*
 * Get the name of the chatbot.
 *
 * Returns: the name of the chatbot as a null-terminated string
 */
const char *chatbot_botname() {

	return "Chatbot";
	
}


/*
 * Get the name of the user.
 *
 * Returns: the name of the user as a null-terminated string
 */
const char *chatbot_username() {

	return "User";
	
}


/*
 * Get a response to user input.
 *
 * See the comment at the top of the file for a description of how this
 * function is used.
 *
 * Returns:
 *   0, if the chatbot should continue chatting
 *   1, if the chatbot should stop (i.e. it detected the EXIT intent)
 */
int chatbot_main(int inc, char *inv[], char *response, int n) {
	
	/* check for empty input */
	if (inc < 1) {
		snprintf(response, n, "");
		return 0;
	}

	/* look for an intent and invoke the corresponding do_* function */
	if (chatbot_is_exit(inv[0]))
		return chatbot_do_exit(inc, inv, response, n);
	else if (chatbot_is_load(inv[0]))
		return chatbot_do_load(inc, inv, response, n);
	else if (chatbot_is_question(inv[0]))
		return chatbot_do_question(inc, inv, response, n);
	else if (chatbot_is_reset(inv[0]))
		return chatbot_do_reset(inc, inv, response, n);
	else if (chatbot_is_save(inv[0]))
		return chatbot_do_save(inc, inv, response, n);
	else if (chatbot_is_smalltalk(inv[0]))
		return chatbot_do_smalltalk(inc, inv, response, n);
	else {
		snprintf(response, n, "I don't understand \"%s\".", inv[0]);
		return 0;
	}

}


/*
 * Determine whether an intent is EXIT.
 *
 * Input:
 *  intent - the intent
 *
 * Returns:
 *  1, if the intent is "exit" or "quit"
 *  0, otherwise
 */
int chatbot_is_exit(const char *intent) {
	
	return compare_token(intent, "exit") == 0 || compare_token(intent, "quit") == 0;
	
}


/*
 * Perform the EXIT intent.
 *
 * See the comment at the top of the file for a description of how this
 * function is used.
 *
 * Returns:
 *   0 (the chatbot always continues chatting after a question)
 */
int chatbot_do_exit(int inc, char *inv[], char *response, int n) {
	
	knowledge_reset();
	snprintf(response, n, "Goodbye!");
	 
	return 1;
	 
}


/*
 * Determine whether an intent is LOAD.
 *
 * Input:
 *  intent - the intent
 *
 * Returns:
 *  1, if the intent is "load"
 *  0, otherwise
 */
int chatbot_is_load(const char *intent) {	

	return (compare_token(intent, "load") == 0 || compare_token(intent, "read") == 0);

}


/*
 * Load a chatbot's knowledge base from a file.
 *
 * See the comment at the top of the file for a description of how this
 * function is used.
 *
 * Returns:
 *   0 (the chatbot always continues chatting after loading knowledge)
 */
int chatbot_do_load(int inc, char *inv[], char *response, int n) {
	int loadedlist;
	char filename[MAX_ENTITY];
	char curr_dir[MAX_PATH];

	if (inc > 1) {
		if (compare_token(inv[1], "from") == 0) {
			if (inc > 2) {
				strcpy(filename, inv[2]);			// Get file name
			}
			else {
				snprintf(response, n, "File not specified.");
				return 0;
			}
		}
		else {
			strcpy(filename, inv[1]);				// Get file name
		}
		
		//checking if filename (entity) is greater than 64 characters
		if (entitycheck(filename)!=0){
			snprintf(response, n, "Filename must be smaller than 64 characters!");
			return 0;
		}

		char *dot = strrchr(filename, '.');			
		if (!(dot && !strcmp(dot, ".ini"))) {		// Check if filename ends with ".ini"
			strcat(filename,".ini");				// If not append ".ini" to end of file
		}
		
		FILE *f = fopen(filename, "r");				// Check if file exists
		if (f == NULL) {
			snprintf(response, n, "File not found.");
			fclose(f);	
			return 0;
		}
		fclose(f);									// Close file
		GetCurrentDirectory(MAX_PATH, curr_dir);	// Get current directory
		strcat(curr_dir, "\\");						// Add backslash to end of cwd
		LPCSTR ini = strcat(curr_dir, filename);	// Get full path of ini file

		loadedlist = knowledge_read(ini);
		snprintf(response, n, "Read %d responses from %s.", loadedlist, filename);
	}
	else {
		snprintf(response, n, "File not specified.");
		return 0;
	}
	return 0;
}


/*
 * Determine whether an intent is a question.
 *
 * Input:
 *  intent - the intent
 *
 * Returns:
 *  1, if the intent is "what", "where", or "who"
 *  0, otherwise
 */
int chatbot_is_question(const char *intent) {
	
	return compare_token(intent, "who") == 0 || compare_token(intent, "where") == 0 || compare_token(intent, "what") == 0;
	
}


/*
 * Answer a question.
 *
 * inv[0] contains the the question word.
 * inv[1] may contain "is" or "are"; if so, it is skipped.
 * The remainder of the words form the entity.
 *
 * See the comment at the top of the file for a description of how this
 * function is used.
 *
 * Returns:
 *   0 (the chatbot always continues chatting after a question)
 */
int chatbot_do_question(int inc, char *inv[], char *response, int n) {
	char* entity[MAX_ENTITY];
	char reply[MAX_RESPONSE] = "I dont know. ";
	char userinput[MAX_RESPONSE] = "";
	int result = 0, result2 = -2; 	//result is for knowledge_get while result2 is for knowledge_put
	int entity_inc = 0;

	for (int i=1; i<inc; ++i) {
		int ignore = 0;
		for (int j=0; j<sizeof(ignoreWords)/sizeof(ignoreWords[0]); ++j) {
			if (compare_token(inv[i],ignoreWords[j]) == 0) {
				ignore = 1;
				break;
			}
		}
		if (ignore == 0) {
			entity[entity_inc] = (char*) malloc(strlen(inv[i])*sizeof(char)+1); 
			strcpy(entity[entity_inc],inv[i]);
			entity_inc++;
		}
	}
	if (entity_inc == 0) {
		snprintf(response, n, "What do you mean?");
		return 0;
	}
	
	char strEntity[MAX_ENTITY] = "";
	for (int i = 0; i < entity_inc; ++i) {
		strcat(strEntity,entity[i]);
		if (i<entity_inc-1) {
			strcat(strEntity," ");
		}
	}
	
	//Checking if entity str is longer than 64 characters
	if(entitycheck(strEntity)!=0){
		snprintf(response, n, "Entity must be smaller than 64 characters!");
		return 0;
	}

	result = knowledge_get(entity_inc, inv[0], entity, response, n);
	
	if (result == KB_INVALID)
		snprintf(response, n, "Error in intent");
	else if (result == KB_NOTFOUND || result == KB_FUZZY){ //get answer from user
		for (int i = 0; i<inc; i++){ // to get everything in inv
			strncat(userinput, inv[i], strlen(inv[i])); //in case there is a single character in response
			if(i<inc-1)
				strcat(userinput, " "); //add spaces between words unless it is the last word
		}
		if (result == KB_NOTFOUND) {
			strcpy(reply, "I don't know. ");
			strcat(reply, userinput); //reply will have "I don't know" plus user response
			strcat(reply, "? (Press enter to skip)");
		}
		else if (result == KB_FUZZY){
			strcpy(reply, "Found a similar question. Press enter if it is correct, else input the correct response.");
		}
		prompt_user(response, n, reply);

		result2 = knowledge_put(inv[0], strEntity, response); // check if user answer is inside knowledge
		if (result2 == KB_OK)
			snprintf(response, n, "Thank you.");
		else if (result2 == KB_INVALID)
			snprintf(response, n, ":-(");
	}
	
	for (int i = 0; i < entity_inc; ++i) {
		if (entity[i] != NULL){
			free(entity[i]);
		}
	}
	
	return 0;
	 
}


/*
 * Determine whether an intent is RESET.
 *
 * Input:
 *  intent - the intent
 *
 * Returns:
 *  1, if the intent is "reset"
 *  0, otherwise
 */
int chatbot_is_reset(const char *intent) {
	
	return compare_token(intent, "reset") == 0;
	
}


/*
 * Reset the chatbot.
 *
 * See the comment at the top of the file for a description of how this
 * function is used.
 *
 * Returns:
 *   0 (the chatbot always continues chatting after beign reset)
 */
int chatbot_do_reset(int inc, char *inv[], char *response, int n) {
	
	knowledge_reset();
	snprintf(response, n, "%s reset", chatbot_botname());
	return 0;
	 
}


/*
 * Determine whether an intent is SAVE.
 *
 * Input:
 *  intent - the intent
 *
 * Returns:
 *  1, if the intent is "what", "where", or "who"
 *  0, otherwise
 */
int chatbot_is_save(const char *intent) {
	
	return (compare_token(intent, "save") == 0 || compare_token(intent, "write") == 0);
	
}


/*
 * Save the chatbot's knowledge to a file.
 *
 * See the comment at the top of the file for a description of how this
 * function is used.
 *
 * Returns:
 *   0 (the chatbot always continues chatting after saving knowledge)
 */
int chatbot_do_save(int inc, char *inv[], char *response, int n) {
	
	char filename[MAX_ENTITY];
	char curr_dir[MAX_PATH];

	if (inc > 1) {
		if (compare_token(inv[1], "as") == 0 || compare_token(inv[1], "to") == 0 ) {
			if (inc > 2) {
				strcpy(filename, inv[2]);			// Get file name
			}
			else {
				snprintf(response, n, "File not specified.");
				return 0;
			}	
		}
		else {
			strcpy(filename, inv[1]);				// Get file name
		}
		
		//checking if filename (entity) is greater than 64 characters
		if (entitycheck(filename)!=0){
			snprintf(response, n, "Filename must be smaller than 64 characters!");
			return 0;
		}
		
		char *dot = strrchr(filename, '.');			
		if (!(dot && !strcmp(dot, ".ini"))) {		// Check if filename ends with ".ini"
			strcat(filename,".ini");				// If not append ".ini" to end of file
		}
		FILE *f = fopen(filename, "a");				// Create file if file does not exist
		fclose(f);									// Close file
		GetCurrentDirectory(MAX_PATH, curr_dir);	// Get current directory
		strcat(curr_dir, "\\");						// Add backslash to end of cwd
		LPCSTR ini = strcat(curr_dir, filename);	// Get full path of ini file

		knowledge_write(ini);
		snprintf(response, n, "My knowledge has been saved to %s.", filename);
	}
	else {
		snprintf(response, n, "File not specified.");
		return 0;
	}
	
	return 0;
}
 
 
/*
 * Determine which an intent is smalltalk.
 *
 *
 * Input:
 *  intent - the intent
 *
 * Returns:
 *  1, if the intent is the first word of one of the smalltalk phrases
 *  0, otherwise
 */
int chatbot_is_smalltalk(const char* intent)
{
	if (stristr(intent, "It") != 0) {
		return 1;
	}

	char* greeting[]= {"Hi","hello","Whatsupp","Good"};
	int num = sizeof(greeting) / sizeof(greeting[0]);
	for(int i=0;i < num;i++){
		if(compare_token(intent,greeting[i])==0)
			return 1;
	}
	return 0;
}

/*
 * Respond to smalltalk.
 *
 * See the comment at the top of the file for a description of how this
 * function is used.
 *
 * Returns:
 *   0, if the chatbot should continue chatting
 *   1, if the chatbot should stop chatting (e.g. the smalltalk was "goodbye" etc.)
 */
int chatbot_do_smalltalk(int inc, char* inv[], char* response, int n)
{
	// few standard response for the bot to use
	char* typeDay[] = {"Morning", "Afternoon", "Evening", "Night" };// for conditional statement later

	char* greetingResponse[]={"Hi","Hello","What's up","Heyyo"};
	int greetingRespSize= sizeof(greetingResponse) / sizeof(greetingResponse[0]);

	time_t rawtime;
    struct tm *current_time;
    time(&rawtime);
    current_time = localtime(&rawtime);

	int isGreeting = 0;
	char currTypeDay[MAX_ENTITY];

	if (compare_token(inv[0],"Good") == 0) {
		if (inc == 1) {
			snprintf(response, n, "Yes, I'm doing good.");
		}
		else {
			for (int i = 0; i < 4; ++i) {
				if (compare_token(inv[1],typeDay[i]) == 0) {
					isGreeting = 1;
					break;
				}
			}
			if (isGreeting) {
				getTypeDay(current_time->tm_hour,currTypeDay);
				if (compare_token(inv[1],currTypeDay) == 0) {
					snprintf(response, n, "Good %s! Need Anything?", currTypeDay);
				}
				else {
					snprintf(response, n, "It is currently %s. Good %s! Need Anything?", currTypeDay, currTypeDay);
				}
			}
			else {
				snprintf(response, n, "What do you mean?");
			}
		}
	}
	else if (stristr(inv[0],"It") != 0) {
		snprintf(response, n, "Indeed it is.");
	}
	else {
		snprintf(response, n, "%s", greetingResponse[rand()%greetingRespSize]); // sending a random response from a list of greeting responses
	}


	return 0;
}

/*
 * Get the typeDay based on current time
 *
 * Input:
 *  currentHour - the current time in hours
 *  typeDay - pointer to a character array containing the typeDay
 *
 */
void getTypeDay (int currentHour, char* typeDay) {
	if (currentHour >= 5 && currentHour <= 11) {
		strcpy(typeDay,"Morning");
	}
	else if (currentHour >= 12 && currentHour <= 16) {
		strcpy(typeDay,"Afternoon");
	}
	else if (currentHour >= 17 && currentHour <= 20) {
		strcpy(typeDay,"Evening");
	}
	else {
		strcpy(typeDay,"Night");
	}
}

// returns 0 if entity < 64 characters
int entitycheck (char *entity){
	if(strlen(entity)<64)
		return 0;
	else
		return 1;
		
}