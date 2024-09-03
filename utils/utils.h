#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <stdbool.h>
#include "delayed_log/delayed_log.h"

#define fail_impl(level, should_abort, delayed, ...)	\
do 														\
{	if(delayed)                                         \
		log_delayed(level " : " __VA_ARGS__);           \
	else												\
		fprintf(stderr, level " : " __VA_ARGS__); 		\
	fputc('\n', stderr);                        		\
	if(should_abort)									\
    {	system("pause");                        		\
        abort();                                		\
    }													\
} while(false)

#define hard_fail(...) fail_impl("error", true, false, __VA_ARGS__)
#define soft_fail(delayed, ...) fail_impl("warning", false, delayed, __VA_ARGS__)