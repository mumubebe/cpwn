#include "log.h"

LogLevel loglevel = LOG_LEVEL_DEBUG;

void set_log_level(LogLevel level)
{
    loglevel = level;
}