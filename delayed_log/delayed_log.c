#include "delayed_log.h"
#include <stdio.h>
#include <malloc.h>
#include <stdarg.h>

struct node { char * message; struct node * next; };

struct node * head = NULL, * last = NULL;

void log_delayed(char * format, ...)
{
	va_list arguments;
	va_start(arguments, format);
	int len = vsnprintf(NULL, 0, format, arguments);
	char * message = malloc(len + 1);
	vsprintf(message, format, arguments);
	va_end(arguments);
	struct node * node = malloc(sizeof(struct node));
	node->message = message;
	node->next = NULL;
	if(head)
	{	last->next = node;
		last = node;
	}
	else
		head = last = node;
}

void release_delayed_log()
{	for(struct node * current = head; current; current = current->next)
		puts(current->message);
}
