/**
 * tests/log
 * 
 * Copyright (C) 2009 Brandon Philips <brandon@ifup.org>
 * 
 * AnekVideo is free software; you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License version
 * 2.1 as published by the Free Software Foundation.
 */

#include "log.h"

int main(int argc, char **argv)
{
	av_log_verbose = 0;
	av_log_debug("debug\n");
	av_log_info("info\n");
	av_log_notice("notice\n");
	av_log_warn("warn\n");
	av_log_error("error\n");

	return 0;
}
