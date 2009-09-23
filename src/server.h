/**
 * server - socket server routines
 * 
 * Copyright (C) 2009 Brandon Philips <brandon@ifup.org>
 * 
 * AnekVideo is free software; you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License version
 * 2.1 as published by the Free Software Foundation.
 */

#ifndef _AV_SERVER_H
#define _AV_SERVER_H

#include "defines.h"
#include "device.h"

#define AV_MAX_DEVICES 32
#define AV_MAX_CLIENTS 32


struct av_server_client {
	int fd;
	struct av_device *device;

	/* if a message isn't completed by timeout close the connection */
	time_t timeout;
	struct av_buffer *msg;

	/* TODO: set to proper length for each ioctl */
	int cur_msg_max;
};

struct av_server {
	fd_set set;
	int max;

	int fd;

	struct av_server_client clients[AV_MAX_CLIENTS];
	/* TODO: make these dynamic lists */
	struct av_device devices[AV_MAX_DEVICES];
};


int av_server_destroy(struct av_server *srv);
struct av_server *av_server_new(void);
int av_server_client_add_by_fd(struct av_server *srv, int fd);
void av_server_client_remove_by_fd(struct av_server *srv, int fd);
int av_server_socket_create(char *path);
int av_server_client_accept(struct av_server *srv);
int av_server_client_handle_request(struct av_server *srv, int fd);

#endif /* _AV_SERVER_H */
