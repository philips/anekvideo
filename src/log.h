/**
 * log - simple logging
 * 
 * Copyright (C) 2009 Brandon Philips <brandon@ifup.org>
 * 
 * AnekVideo is free software; you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License version
 * 2.1 as published by the Free Software Foundation.
 */

#ifndef _AV_LOG_H
#define _AV_LOG_H
#include <stdio.h>

int av_log_verbose;

enum av_log_level {
	AV_LOG_ERROR  = 0,
	AV_LOG_WARN   = 1,
	AV_LOG_NOTICE = 2,
	AV_LOG_INFO   = 3,
	AV_LOG_DEBUG  = 4,
};

#define av_log_level_meta(lvl, file, line, func, format, ...)	\
	if (av_log_verbose) fprintf(stderr, "%i %s:%i %s() " format, lvl, file, line, func, ##__VA_ARGS__); \
	else fprintf(stderr, "%s() " format, func, ##__VA_ARGS__);

#define av_log_debug(...)  av_log_level_meta(AV_LOG_DEBUG,  __FILE__, __LINE__, __func__, __VA_ARGS__)
#define av_log_info(...)   av_log_level_meta(AV_LOG_INFO,   __FILE__, __LINE__, __func__, __VA_ARGS__)
#define av_log_notice(...) av_log_level_meta(AV_LOG_NOTICE, __FILE__, __LINE__, __func__, __VA_ARGS__)
#define av_log_warn(...)   av_log_level_meta(AV_LOG_WARN,   __FILE__, __LINE__, __func__, __VA_ARGS__)
#define av_log_error(...)  av_log_level_meta(AV_LOG_ERROR,  __FILE__, __LINE__, __func__, __VA_ARGS__)

#define av_log_error_and_exit(code, ...) while(1) { av_log_level_meta(AV_LOG_ERROR,  __FILE__, __LINE__, __func__, __VA_ARGS__); exit(code); }

#endif /* _AV_LOG_H */
