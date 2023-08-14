#ifndef LOG_H
#define LOG_H

#include <stdio.h>


typedef enum {
    LOG_LEVEL_INFO,
    LOG_LEVEL_DEBUG,
} LogLevel;

extern LogLevel loglevel;

#define LOG_INFO(format, ...) do { if (loglevel >= LOG_LEVEL_INFO) printf("[INFO] " format "\n", ##__VA_ARGS__); } while(0)
#define LOG_DEBUG(format, ...) do { if (loglevel >= LOG_LEVEL_DEBUG) printf("[DEBUG] " format "\n", ##__VA_ARGS__); } while(0)

extern void set_log_level (LogLevel level);

#endif