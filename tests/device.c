/**
 * tests/device
 * 
 * Copyright (C) 2009 Brandon Philips <brandon@ifup.org>
 * 
 * AnekVideo is free software; you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License version
 * 2.1 as published by the Free Software Foundation.
 */

#include "defines.h"
#include "device.h"
#include "log.h"

int main(int argc, char **argv)
{
	struct av_server *srv = av_server_new();
	char path[] = "/dev/video0";
	char bad_path[] = "/anekvideorocks";

	if (!srv) av_log_error_and_exit(1, "av_server_new failed");
	av_device_new_from_path(srv, bad_path);

	av_device_new_from_path(srv, path);

	return 0;
}
