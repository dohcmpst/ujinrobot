/**
  ******************************************************************************
  * @file           : mpst_log.c
  * @brief          : log library
  ******************************************************************************
  ******************************************************************************
  */
/* Includes ------------------------------------------------------------------*/


#include "mpst_log.h"

static unsigned char u8s_LogLevel = LOG_INFO;
static unsigned char u8s_LogQuiet = 0;
static unsigned char u8s_LogColorEn = 1;

char * type[LOG_LEVEL_LEN] = {
	"TRACE", "DEBUG", "INFO", "WARN", "ERROR", "FATAL"   
}; 

char * colors[LOG_LEVEL_LEN] = {
    "\x1b[94m", "\x1b[0m", "\x1b[32m", "\x1b[33m", "\x1b[31m", "\x1b[35m"
};



void mpst_log_set(unsigned char u8_level, unsigned char u8_quiet, unsigned char u8_colorEn) {
  u8s_LogLevel = u8_level;
  u8s_LogQuiet = u8_quiet;
  u8s_LogColorEn = u8_colorEn;
}


void mpst_log(LOG_LEVEL level, const char *file, int line, const char *fmt, ...) {

 	va_list args;
    va_start(args, fmt);


	if(!u8s_LogQuiet && level >= u8s_LogLevel) 
	{

		if(u8s_LogColorEn) 
		{
        	printf("%s", colors[level]);
		}

		printf("[%s-%s:%d] ", type[level], file, line);
		vprintf(fmt, args);

		if(u8s_LogColorEn) 
		{
			printf("%s", colors[LOG_DEBUG]);
		}
	}

    va_end(args);
}
