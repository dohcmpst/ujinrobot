/**
  ******************************************************************************
  * @file           : mpst_log.h
  * @brief          : log library
  ******************************************************************************
  *
  ******************************************************************************
  */
#ifndef __MPST_LOG_H__
#define __MPST_LOG_H__

#include <stdio.h>
#include <stdarg.h>


#define LOG_VERSION "0.0.1" 


typedef enum {
    LOG_TRACE,
	LOG_DEBUG,
    LOG_INFO,
    LOG_WARN,
    LOG_ERROR,
	LOG_FATAL,
    LOG_LEVEL_LEN
} LOG_LEVEL;



#define log_trace(...) mpst_log(LOG_TRACE, __FILE__, __LINE__, __VA_ARGS__)
#define log_debug(...) mpst_log(LOG_DEBUG, __FILE__, __LINE__, __VA_ARGS__)
#define log_info(...)  mpst_log(LOG_INFO,  __FILE__, __LINE__, __VA_ARGS__)
#define log_warn(...)  mpst_log(LOG_WARN,  __FILE__, __LINE__, __VA_ARGS__)
#define log_error(...) mpst_log(LOG_ERROR, __FILE__, __LINE__, __VA_ARGS__)
#define log_fatal(...) mpst_log(LOG_FATAL, __FILE__, __LINE__, __VA_ARGS__)


void mpst_log_set(unsigned char u8_level, unsigned char u8_quiet, unsigned char u8_colorEn);

void mpst_log(LOG_LEVEL level, const char *file, int line, const char *fmt, ...);



#endif /* __MPST_LOG_H__ */
