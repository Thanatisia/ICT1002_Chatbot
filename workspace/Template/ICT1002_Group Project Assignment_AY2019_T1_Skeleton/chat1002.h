/*
 * ICT1002 (C Language) Group Project, AY19 Trimester 1.
 *
 * This file contains the definitions and function prototypes for all of
 * features of the ICT1002 chatbot.
 */
 
#ifndef _CHAT1002_H
#define _CHAT1002_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <time.h>
#include "Windows.h"
#include "ctype.h"


/* the maximum number of characters we expect in a line of input (including the terminating null)  */
#define MAX_INPUT    256

/* the maximum number of characters allowed in the name of an intent (including the terminating null)  */
#define MAX_INTENT   32

/* the maximum number of characters allowed in the name of an entity (including the terminating null)  */
#define MAX_ENTITY   64

/* the maximum number of characters allowed in a response (including the terminating null) */
#define MAX_RESPONSE 277 //need to include "I don't understand" + MAX_INPUT amount of words

/* the maximum number of characters allowed for all the keys in a section for the ini file */
#define MAX_KEYS 2000

 /* size of each intent node array (26 alphabets + nonalpha characters) */
#define NODE_SIZE 27

/* return codes for knowledge_get() and knowledge_put() */
#define KB_OK        0
#define KB_NOTFOUND -1
#define KB_INVALID  -2
#define KB_NOMEM    -3
#define KB_FUZZY    -4
 
/* functions defined in main.c */
int compare_token(const char *token1, const char *token2);
void prompt_user(char *buf, int n, const char *format, ...);

/* functions defined in chatbot.c */
const char *chatbot_botname();
const char *chatbot_username();
int chatbot_main(int inc, char *inv[], char *response, int n);
int chatbot_is_exit(const char *intent);
int chatbot_do_exit(int inc, char *inv[], char *response, int n);
int chatbot_is_load(const char *intent);
int chatbot_do_load(int inc, char *inv[], char *response, int n);
int chatbot_is_question(const char *intent);
int chatbot_do_question(int inc, char *inv[], char *response, int n);
int chatbot_is_reset(const char *intent);
int chatbot_do_reset(int inc, char *inv[], char *response, int n);
int chatbot_is_save(const char *intent);
int chatbot_do_save(int inc, char *inv[], char *response, int n);
int chatbot_is_smalltalk(const char *intent);
int chatbot_do_smalltalk(int inc, char *inv[], char *resonse, int n);
void getTypeDay (int currentHour, char* typeDay);
int entitycheck (char *entity);

/* functions defined in knowledge.c */
int knowledge_get(int inc, const char *intent, char *entity[], char *response, int n);
int knowledge_put(const char *intent, const char *entity, const char *response);
void knowledge_reset();
int knowledge_read(LPCSTR ini);
int read_section (char *section, LPCSTR ini);
void knowledge_write(LPCSTR ini);
void write_section (char *section, LPCSTR ini);
void toLowerStr(char *str); 
char* stristr( const char* str1, const char* str2 ); 

#endif