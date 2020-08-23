#include "debug.h"

#include "config.h"

void debug(int level, const char *format, ...)
{
    if (getConfig()->debugMode >= level)
    {
        va_list args;
        va_start(args, format);
        vprintf(format, args);
        va_end(args);
    }
}
