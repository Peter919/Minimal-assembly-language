#pragma once

enum e_LogLevel {
        LOG_DEBUG,
        LOG_INFO,
        LOG_SUCCESS,
        LOG_WARNING,
        LOG_ERROR,
        LOG_FATAL_ERROR,
        LOG_NONE
};

extern const int min_log_lvl;

char logger(enum e_LogLevel log_level, const char * text, ...);

char newlines(enum e_LogLevel log_level, unsigned int amount);
